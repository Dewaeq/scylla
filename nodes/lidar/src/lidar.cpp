#include "lidar.hpp"
#include <iostream>

using namespace sl;

Lidar::Lidar(const std::string &device) {
  const auto channel = createSerialPortChannel(device, 115200);
  if (SL_IS_FAIL(channel.err)) {
    std::cout << "[FATAL] failed to create serial port channel" << std::endl;
    exit(-1);
  }

  this->channel = channel.value;

  const auto driver = createLidarDriver();
  if (!driver) {
    std::cout << "[FATAL] failed to create lidar driver" << std::endl;
    exit(-1);
  }

  this->driver = driver.value;
}

bool Lidar::connect() {
  const auto result = this->driver->connect(this->channel);
  return SL_IS_OK(result);
}
