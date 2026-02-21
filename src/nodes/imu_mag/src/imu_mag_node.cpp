#include "imu_mag/imu_mag_node.hpp"

#include "common/lcm_node.hpp"
#include "imu_mag/imu_mag_node.hpp"
#include "lis3mdl/lis3mdl_driver.hpp"
#include "scylla_msgs/imu_t.hpp"
#include "scylla_msgs/mag_t.hpp"
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

  static scylla_msgs::imu_t imu_msg;
  static scylla_msgs::mag_t mag_msg;

  if (imu_driver_.has_data() && imu_driver_.read(imu_data)) {
    imu_msg.timestamp = now_ns();
    imu_msg.ax = imu_data.ax;
    imu_msg.ay = imu_data.ay;
    imu_msg.az = imu_data.az;

    imu_msg.gx = imu_data.gx;
    imu_msg.gy = imu_data.gy;
    imu_msg.gz = imu_data.gz;
    publish("sensors/imu", imu_msg);
  }
  if (mag_driver_.has_data() && mag_driver_.read(mag_data)) {
    mag_msg.timestamp = now_ns();
    mag_msg.mx = mag_data.mx;
    mag_msg.my = mag_data.my;
    mag_msg.mz = mag_data.mz;
    publish("sensors/mag", mag_msg);
  }
}
