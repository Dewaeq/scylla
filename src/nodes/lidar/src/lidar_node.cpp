#include "lidar/lidar_node.hpp"

#include "common/lcm_node.hpp"
#include <chrono>
#include <cstdio>
#include <sstream>
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
      sprintf(info_str.data(),
              "Model: %d, Firmware Version: %d.%d, Hardware Version: %d\n",
              deviceInfo.model, deviceInfo.firmware_version >> 8,
              deviceInfo.firmware_version & 0xffu, deviceInfo.hardware_version);
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
    std::stringstream ss;
    ss << scan_mode.scan_mode << "\t" << "max dist: " << scan_mode.max_distance
       << "  µs: " << scan_mode.us_per_sample;
    info(ss.str());
  }

  driver_->reset();
  // give it a moment
  std::this_thread::sleep_for(std::chrono::seconds(1));

  LidarScanMode scan_mode;
  driver_->startScan(false, true, 0, &scan_mode);
  info("default scan mode: " + std::string(scan_mode.scan_mode));
}

void LidarNode::update() {
  static rplidar_response_measurement_node_hq_t nodes[8192];
  size_t count = 8192;

  auto result = driver_->grabScanDataHq(nodes, count);

  if (IS_OK(result)) {
    driver_->ascendScanData(nodes, count);
  }
}

LidarNode::~LidarNode() {
  delete channel_;
  delete driver_;
}
