#include "imu_mag/imu_mag_node.hpp"

#include "common/lcm_node.hpp"
#include "imu_mag/imu_mag_node.hpp"
#include <sstream>
#include <wiringPi.h>

ImuMagNode::ImuMagNode() : LcmNode("imu_mag_node") {
  if (wiringPiSetupGpio() != 0) {
    error("wiringPi GPIO setup failed");
    exit(-1);
  }

  info("power cyling LSM6DSOX/LIS3MDL board...");
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);
  delay(100);
  digitalWrite(POWER_PIN, HIGH);
  delay(100);

  if (!imu_driver_.begin()) {
    error("failed to init imu driver!");
    exit(-1);
  }
  info("imu driver initialised");

  if (!mag_driver_.begin()) {
    error("failed to init mag driver!");
    exit(-1);
  }
  info("mag driver initialised");
}

void ImuMagNode::update() {
  if (imu_driver_.has_data()) {
    const auto data = imu_driver_.read();
    std::stringstream s;
    s << "[imu data]: "
      << "\tax: " << data.ax << "\tay: " << data.ay << "\taz: " << data.az
      << std::endl
      << "\tgx: " << data.gx << "\tgy: " << data.gy << "\tgz: " << data.gz;
    // info(s.str());
  }
  if (mag_driver_.has_data()) {
    const auto data = mag_driver_.read();
    std::stringstream s;
    s << "[mag data]: "
      << "\tmx: " << data.mx << "\tmy: " << data.my << "\tmz: " << data.mz;
    info(s.str());
  }
}
