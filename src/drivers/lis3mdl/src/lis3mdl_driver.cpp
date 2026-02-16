#include "lis3mdl/lis3mdl_driver.hpp"

#include <cstdint>
#include <iostream>
#include <wiringPi.h>
#include <wiringPiI2C.h>

Lis3mdlDriver::Lis3mdlDriver() {}

bool Lis3mdlDriver::begin() {
  if (wiringPiSetupGpio() != 0) {
    std::cerr << "wiringPi GPIO setup failed" << std::endl;
    return false;
  }

  fd_ = wiringPiI2CSetup(LIS3MDL_ADDR);
  if (fd_ < 0) {
    std::cerr << "wiringPi LIS3MDL IC2 setup failed" << std::endl;
    return false;
  }

  // verify ID
  int who = wiringPiI2CReadReg8(fd_, LIS3MDL_WHO_AM_I);
  if (who != 0x3D) {
    std::cout << "wrong LIS3MDL id: got 0x" << std::hex << who << std::endl;
    return false;
  }

  // start magnetometer, see table 18
  // no temp sensor, ultrahigh-performance mode, 10 Hz
  wiringPiI2CWriteReg8(fd_, LIS3MDL_CTRL_REG1, 0x70);
  // +- 4 gauss scale
  wiringPiI2CWriteReg8(fd_, LIS3MDL_CTRL_REG2, 0x00);
  // continuous-conversion mode
  wiringPiI2CWriteReg8(fd_, LIS3MDL_CTRL_REG3, 0x00);

  return true;
}

bool Lis3mdlDriver::has_data() {
  const int status = wiringPiI2CReadReg8(fd_, LIS3MDL_STATUS_REG);
  if (status < 0)
    return false;
  return (status & 0x08) == 0x08;
}

Lis3mdlData Lis3mdlDriver::read() {
  Lis3mdlData data;

  int16_t raw_mx = wiringPiI2CReadReg16(fd_, LIS3MDL_OUT_X_L | 0x80);
  int16_t raw_my = wiringPiI2CReadReg16(fd_, (LIS3MDL_OUT_X_L + 2) | 0x80);
  int16_t raw_mz = wiringPiI2CReadReg16(fd_, (LIS3MDL_OUT_X_L + 4) | 0x80);

  // 4 gauss range -> 6842 LSB/gauss (table 2)
  // 1 Gauss = 100 uTesla
  const float scale = 100.0f / 6842.0f;
  data.mx = raw_mx * scale;
  data.my = raw_my * scale;
  data.mz = raw_mz * scale;

  return data;
}
