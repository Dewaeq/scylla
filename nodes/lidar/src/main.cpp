#include <chrono>
#include <cmath>
#include <iostream>
#include <lcm/lcm-cpp.hpp>
#include <numbers>
#include <thread>
#include <unistd.h>
#include <vector>

#include "lidar.hpp"
#include "messages/LaserScan.hpp"
#include "rplidar.h"
#include "scan_filter.hpp"
#include "sl_lidar.h"
#include "sl_lidar_driver.h"

using namespace sl;

int64_t now_ns() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

int main() {
  ScanFilter scan_filter;

  const auto channel = createSerialPortChannel("/dev/ttyUSB0", 115200);
  if (SL_IS_FAIL(channel.err))
    exit(-1);

  const auto driver = createLidarDriver();
  if (!driver)
    exit(-1);

  ILidarDriver *const lidar = driver.value;

  const auto res = lidar->connect(channel.value);

  if (SL_IS_OK(res)) {
    sl_lidar_response_device_info_t deviceInfo;
    const auto res = lidar->getDeviceInfo(deviceInfo);
    if (SL_IS_OK(res)) {
      printf("Model: %d, Firmware Version: %d.%d, Hardware Version: %d\n",
             deviceInfo.model, deviceInfo.firmware_version >> 8,
             deviceInfo.firmware_version & 0xffu, deviceInfo.hardware_version);
    } else {
      fprintf(stderr, "Failed to get device information from LIDAR %08x\r\n",
              res);
      return -1;
    }
  } else {
    fprintf(stderr, "Failed to connect to LIDAR %08x\r\n", res);
    return -1;
  }

  std::vector<LidarScanMode> scan_modes;
  lidar->getAllSupportedScanModes(scan_modes);
  for (const auto &scan_mode : scan_modes) {
    std::cout << scan_mode.scan_mode << std::endl;
    std::cout << "\t" << "max dist: " << scan_mode.max_distance
              << "  us: " << scan_mode.us_per_sample << std::endl;
  }

  lidar->reset();
  // give it a moment
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  LidarScanMode scan_mode;
  lidar->startScan(false, true, 0, &scan_mode);
  std::cout << "default" << std::endl;
  std::cout << scan_mode.scan_mode << std::endl;
  std::cout << "\t" << "max dist: " << scan_mode.max_distance
            << "  us: " << scan_mode.us_per_sample << std::endl;

  lcm::LCM lcm;
  if (!lcm.good()) {
    std::cerr << "failed to init LCM!" << std::endl;
    return -1;
  }

  sl_lidar_response_measurement_node_hq_t nodes[8192];
  size_t count = 8192;

  while (1) {
    auto result = lidar->grabScanDataHq(nodes, count);
    if (SL_IS_FAIL(result) || result == SL_RESULT_OPERATION_TIMEOUT) {
      std::cerr << "failed read lidar" << std::endl;
      sleep(1);
      continue;
    }

    messages::LaserScan msg;
    msg.timestamp = now_ns();

    lidar->ascendScanData(nodes, count);
    for (int i = 0; i < count; i++) {
      const auto &node = nodes[i];
      const float dist = node.dist_mm_q2 / 4.0f;
      const float deg = node.angle_z_q14 * 90. / 16384.;
      const float rad = deg * std::numbers::pi / 180.;

      messages::ScanPoint point;
      point.x = dist * std::sin(rad);
      point.y = dist * std::cos(rad);
      msg.points.push_back(point);
    }
    msg.num_points = msg.points.size();
    const auto old_count = msg.num_points;

    lcm.publish("laser_scan", &msg);

    scan_filter.filter(msg);
    const auto new_count = msg.num_points;

    std::cout << "[INFO] filtered out " << old_count - new_count
              << " points from " << old_count << std::endl;

    lcm.publish("filtered_scan", &msg);
  }

  lidar->setMotorSpeed(0);
  lidar->stop();

  return 0;
}
