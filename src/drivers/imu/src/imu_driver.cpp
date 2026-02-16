#include "imu/imu_driver.hpp"

#include <cmath>
#include <iostream>
#include <wiringPi.h>
#include <wiringPiI2C.h>

ImuDriver::ImuDriver() {}

int ImuDriver::begin() {
  if (wiringPiSetupGpio() != 0) {
    std::cerr << "wiringPi GPIO setup failed" << std::endl;
    return -1;
  }

  std::cout << "power cycling IMU..." << std::endl;
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);
  delay(100);
  digitalWrite(POWER_PIN, HIGH);
  delay(100);

  fd_imu_ = wiringPiI2CSetup(LSM6DSOX_ADDR);
  if (fd_imu_ < 0) {
    std::cerr << "wiringPi LSM6DSOX I2C setup failed" << std::endl;
    return -1;
  }

  // verify ID
  int who_imu = wiringPiI2CReadReg8(fd_imu_, LSM6DSOX_WHO_AM_I);
  if (who_imu != 0x6C) {
    std::cout << "wrong LSM6DSOX id: got 0x" << std::hex << who_imu
              << std::endl;
    return -1;
  }

  fd_mag_ = wiringPiI2CSetup(LIS3MDL_ADDR);
  if (fd_mag_ < 0) {
    std::cerr << "wiringPi LIS3MDL IC2 setup failed" << std::endl;
    return -1;
  }

  wiringPiI2CWriteReg8(fd_mag_, LIS3MDL_CTRL_REG2, 0x04);
  delay(10);

  // verify ID
  int who_mag = wiringPiI2CReadReg8(fd_mag_, LIS3MDL_WHO_AM_I);
  if (who_mag != 0x3D) {
    std::cout << "wrong LIS3MDL id: got 0x" << std::hex << who_mag << std::endl;
    return -1;
  }

  // start accelerometer, see table 51 in datasheet
  // 208 Hz normal mode, +- 2g, no digital filter
  wiringPiI2CWriteReg8(fd_imu_, LSM6DSOX_CTRL1_XL, 0x50);
  // start gyro, see table 55 in datasheet
  // 208 Hz normal mode with 250 dps
  wiringPiI2CWriteReg8(fd_imu_, LSM6DSOX_CTRL2_G, 0x50);

  // start magnetometer, see table 18
  // no temp sensor, ultrahigh-performance mode, 10 Hz
  wiringPiI2CWriteReg8(fd_mag_, LIS3MDL_CTRL_REG1, 0x70);
  // +- 4 gauss scale
  wiringPiI2CWriteReg8(fd_mag_, LIS3MDL_CTRL_REG2, 0x00);
  // continuous-conversion mode
  wiringPiI2CWriteReg8(fd_mag_, LIS3MDL_CTRL_REG3, 0x00);

  return 0;
}

ImuData ImuDriver::read_data() {
  ImuData data;

  // gyro
  int16_t raw_gx = (int16_t)wiringPiI2CReadReg16(fd_imu_, LSM6DSOX_OUTX_L_G);
  int16_t raw_gy =
      (int16_t)wiringPiI2CReadReg16(fd_imu_, LSM6DSOX_OUTX_L_G + 2);
  int16_t raw_gz =
      (int16_t)wiringPiI2CReadReg16(fd_imu_, LSM6DSOX_OUTX_L_G + 4);

  // accelerometer
  int16_t raw_ax = (int16_t)wiringPiI2CReadReg16(fd_imu_, LSM6DSOX_OUTX_L_A);
  int16_t raw_ay =
      (int16_t)wiringPiI2CReadReg16(fd_imu_, LSM6DSOX_OUTX_L_A + 2);
  int16_t raw_az =
      (int16_t)wiringPiI2CReadReg16(fd_imu_, LSM6DSOX_OUTX_L_A + 4);

  // magnetometer
  int16_t raw_mx =
      (int16_t)wiringPiI2CReadReg16(fd_mag_, LIS3MDL_OUT_X_L | 0x80);
  int16_t raw_my =
      (int16_t)wiringPiI2CReadReg16(fd_mag_, (LIS3MDL_OUT_X_L + 2) | 0x80);
  int16_t raw_mz =
      (int16_t)wiringPiI2CReadReg16(fd_mag_, (LIS3MDL_OUT_X_L + 4) | 0x80);

  // conversions
  // Accel: 2g range -> 0.061 mg/LSB (table 2)
  const float accel_scale = 0.061f * 9.81f / 1000.0f;
  data.ax = raw_ax * accel_scale;
  data.ay = raw_ay * accel_scale;
  data.az = raw_az * accel_scale;

  // Gyro: 250dps range -> 8.75 mdps/LSB (table 2)
  const float gyro_scale = 8.75f / 1000.0f * M_PI / 180.0f;
  data.gx = raw_gx * gyro_scale;
  data.gy = raw_gy * gyro_scale;
  data.gz = raw_gz * gyro_scale;

  // Mag: 4 gauss range -> 6842 LSB/gauss (table 2)
  // 1 Gauss = 100 uTesla
  const float mag_scale = 100.0f / 6842.0f;
  data.mx = raw_mx * mag_scale;
  data.my = raw_my * mag_scale;
  data.mz = raw_mz * mag_scale;

  return data;
}
