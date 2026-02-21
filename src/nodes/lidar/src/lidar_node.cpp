#include "lidar/lidar_node.hpp"

#include "common/lcm_node.hpp"
#include "scylla_msgs/lidar_t.hpp"
#include "sl_lidar_driver.h"
#include <chrono>
#include <cmath>
#include <cstdio>
#include <fmt/core.h>
#include <thread>

LidarNode::LidarNode() : LcmNode("lidar_node") {
  auto channel = createSerialPortChannel("/dev/ttyUSB0", 115200);
  if (!channel) {
    error("failed to create lidar channel");
    exit(-1);
  }
  channel_ = *channel;

  auto driver = createLidarDriver();
  if (!driver) {
    error("failed to create lidar driver");
    exit(-1);
  }
  driver_ = *driver;

  auto res = driver_->connect(channel_);
  if (SL_IS_OK(res)) {
    sl_lidar_response_device_info_t deviceInfo;
    auto res = driver_->getDeviceInfo(deviceInfo);
    if (SL_IS_OK(res)) {
      std::string info_str;
      info(std::format(
          "Model: {}, Firmware Version: {}.{}, Hardware Version: {}\n",
          deviceInfo.model, deviceInfo.firmware_version >> 8,
          deviceInfo.firmware_version & 0xffu, deviceInfo.hardware_version));
      info(info_str);
    } else {
      error("failed to get device information from lidar: " +
            std::to_string(res));
      exit(-1);
    }
  } else {
    error("failed to connect to lidar: " + std::to_string(res));
    exit(-1);
  }

  std::vector<LidarScanMode> scan_modes;
  driver_->getAllSupportedScanModes(scan_modes);

  info("Lidar scan modes:");
  for (const auto &scan_mode : scan_modes) {
    info(fmt::format("{}\t max dist: {}  µs: {}", scan_mode.scan_mode,
                     scan_mode.max_distance, scan_mode.us_per_sample));
  }

  driver_->reset();
  // give it a moment
  std::this_thread::sleep_for(std::chrono::seconds(1));

  LidarScanMode scan_mode;
  driver_->startScan(false, true, 0, &scan_mode);
  LidarMotorInfo motor_info;
  driver_->getMotorInfo(motor_info);

  info(fmt::format("Motor info: min speed={}, max speed={}, desired speed={}",
                   motor_info.min_speed, motor_info.max_speed,
                   motor_info.desired_speed));

  driver_->setMotorSpeed();
  info("starting scan in mode: " + std::string(scan_mode.scan_mode));
}

void LidarNode::update() {
  static rplidar_response_measurement_node_hq_t nodes[8192];
  size_t count = 8192;

  auto result = driver_->grabScanDataHq(nodes, count);

  if (IS_OK(result)) {
    driver_->ascendScanData(nodes, count);

    scylla_msgs::lidar_t msg;
    msg.timestamp = now_ns();

    msg.num_points = count;
    msg.ranges.resize(count);
    msg.angles.resize(count);
    msg.intensities.resize(count);

    for (size_t i = 0; i < count; ++i) {
      float angle_rad = nodes[i].angle_z_q14 * 90.f / 16384.f * (M_PI / 180.0f);
      float dist_m = nodes[i].dist_mm_q2 / 4.0f / 1000.0f;
      float quality = nodes[i].quality;

      if (dist_m < 0.05 || dist_m > 10.0)
        continue;

      msg.ranges[i] = dist_m;
      msg.angles[i] = angle_rad;
      msg.intensities[i] = quality;
    }

    publish("sensors/lidar", msg);
  } else {
    error("failed to read lidar data: " + std::to_string(result));
  }
}

LidarNode::~LidarNode() {
  delete channel_;
  delete driver_;
}
