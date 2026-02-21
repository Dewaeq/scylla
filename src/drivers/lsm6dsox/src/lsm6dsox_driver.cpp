#include "lsm6dsox/lsm6dsox_driver.hpp"

#include <cstdint>
#include <iostream>
#include <numbers>
#include <wiringPi.h>
#include <wiringPiI2C.h>

Lsm6dsoxDriver::Lsm6dsoxDriver() {}

bool Lsm6dsoxDriver::begin() {
  if (wiringPiSetupGpio() != 0) {
    std::cerr << "wiringPi GPIO setup failed" << std::endl;
    return false;
  }

  fd_ = wiringPiI2CSetup(LSM6DSOX_ADDR);
  if (fd_ < 0) {
    std::cerr << "wiringPi LSM6DSOX I2C setup failed" << std::endl;
    return false;
  }

  // verify ID
  int who = wiringPiI2CReadReg8(fd_, LSM6DSOX_WHO_AM_I);
  if (who != 0x6C) {
    std::cout << "wrong LSM6DSOX id: got 0x" << std::hex << who << std::endl;
    return false;
  }

  // start accelerometer, see table 51 in datasheet
  // 208 Hz normal mode, +- 2g, no digital filter
  wiringPiI2CWriteReg8(fd_, LSM6DSOX_CTRL1_XL, 0x50);
  // start gyro, see table 55 in datasheet
  // 208 Hz normal mode with 250 dps
  wiringPiI2CWriteReg8(fd_, LSM6DSOX_CTRL2_G, 0x50);

  return true;
}

bool Lsm6dsoxDriver::has_data() {
  const int status = wiringPiI2CReadReg8(fd_, LSM6DSOX_STATUS_REG);
  if (status < 0)
    return false;
  // bit 0: accelerometer
  // bit 1: gyroscope
  // bit 2: temperature
  return (status & 0x03) == 0x03;
}

bool Lsm6dsoxDriver::read(Lsm6dsoxData &data) {
  // gyro
  int16_t raw_gx = wiringPiI2CReadReg16(fd_, LSM6DSOX_OUTX_L_G);
  int16_t raw_gy = wiringPiI2CReadReg16(fd_, LSM6DSOX_OUTX_L_G + 2);
  int16_t raw_gz = wiringPiI2CReadReg16(fd_, LSM6DSOX_OUTX_L_G + 4);

  if (raw_gx == -1 || raw_gy == -1 || raw_gz == -1)
    return false;

  // accelerometer
  int16_t raw_ax = wiringPiI2CReadReg16(fd_, LSM6DSOX_OUTX_L_A);
  int16_t raw_ay = wiringPiI2CReadReg16(fd_, LSM6DSOX_OUTX_L_A + 2);
  int16_t raw_az = wiringPiI2CReadReg16(fd_, LSM6DSOX_OUTX_L_A + 4);

  if (raw_ax == -1 || raw_ay == -1 || raw_az == -1)
    return false;

  // conversions
  // Accel: 2g range -> 0.061 mg/LSB (table 2)
  const float accel_scale = 0.061f * 9.81f / 1000.0f;
  data.ax = raw_ax * accel_scale;
  data.ay = raw_ay * accel_scale;
  data.az = raw_az * accel_scale;

  // Gyro: 250dps range -> 8.75 mdps/LSB (table 2)
  const float gyro_scale = 8.75f / 1000.0f * std::numbers::pi / 180.0f;
  data.gx = raw_gx * gyro_scale;
  data.gy = raw_gy * gyro_scale;
  data.gz = raw_gz * gyro_scale;

  return true;
}
