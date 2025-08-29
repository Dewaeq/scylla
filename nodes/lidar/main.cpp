#include <chrono>
#include <iostream>
#include <lcm/lcm-cpp.hpp>
#include <unistd.h>
#include <vector>

#include "messages/LaserScan.hpp"
#include "rplidar.h"
#include "sl_lidar.h"
#include "sl_lidar_driver.h"

using namespace sl;

int64_t now_ns() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

int main() {
  auto channel = createSerialPortChannel("/dev/ttyUSB0", 115200);
  if (SL_IS_FAIL(channel.err))
    exit(-1);

  auto driver = createLidarDriver();
  if (!driver)
    exit(-1);

  ILidarDriver *lidar = driver.value;

  auto res = lidar->connect(channel.value);

  if (SL_IS_OK(res)) {
    sl_lidar_response_device_info_t deviceInfo;
    res = lidar->getDeviceInfo(deviceInfo);
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

  LidarScanMode scan_mode;
  lidar->startScan(false, true, 0, &scan_mode);
  std::cout << "default" << std::endl;
  std::cout << scan_mode.scan_mode << std::endl;
  std::cout << "\t" << "max dist: " << scan_mode.max_distance
            << "  us: " << scan_mode.us_per_sample << std::endl;

  lcm::LCM lcm;
  if (!lcm.good())
    return -1;

  sl_lidar_response_measurement_node_hq_t nodes[8192];
  size_t count = 8192;

  while (1) {
    auto result = lidar->grabScanDataHq(nodes, count);
    if (SL_IS_FAIL(result) || result == SL_RESULT_OPERATION_TIMEOUT) {
      return -1;
    }

    std::cout << "read " << count << " points" << std::endl;

    messages::LaserScan msg;
    msg.timestamp = now_ns();
    msg.num_points = count;
    msg.points.resize(count);

    lidar->ascendScanData(nodes, count);
    for (int i = 0; i < count; i++) {
      msg.points[i].angle = nodes[i].angle_z_q14 * 90.0f / 16384.0f;
      msg.points[i].distance = nodes[i].dist_mm_q2 / 4.0f;
    }

    lcm.publish("scans", &msg);
  }

  lidar->setMotorSpeed(0);
  lidar->stop();

  sleep(5);

  return 0;
}
