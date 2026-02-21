#include "imu_mag/imu_mag_node.hpp"

#include "common/lcm_node.hpp"
#include "imu_mag/imu_mag_node.hpp"
#include "lis3mdl/lis3mdl_driver.hpp"
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
  static Lsm6dsoxData imu_data;
  static Lis3mdlData mag_data;

  if (imu_driver_.has_data() && imu_driver_.read(imu_data)) {
    std::stringstream s;
    s << "[imu data]: "
      << "\tax: " << imu_data.ax << "\tay: " << imu_data.ay
      << "\taz: " << imu_data.az << std::endl
      << "\tgx: " << imu_data.gx << "\tgy: " << imu_data.gy
      << "\tgz: " << imu_data.gz;
    // info(s.str());
  }
  if (mag_driver_.has_data() && mag_driver_.read(mag_data)) {
    std::stringstream s;
    s << "[mag data]: "
      << "\tmx: " << mag_data.mx << "\tmy: " << mag_data.my
      << "\tmz: " << mag_data.mz;
    info(s.str());
  }
}
