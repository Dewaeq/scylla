#pragma once

struct Lis3mdlData {
  float mx, my, mz;
};

class Lis3mdlDriver {
public:
  Lis3mdlDriver();

  bool begin();
  bool has_data();
  Lis3mdlData read();

private:
  const int LIS3MDL_ADDR = 0x1C;
  const int LIS3MDL_WHO_AM_I = 0x0F;
  const int LIS3MDL_CTRL_REG1 = 0x20;
  const int LIS3MDL_CTRL_REG2 = 0x21;
  const int LIS3MDL_CTRL_REG3 = 0x22;
  const int LIS3MDL_OUT_X_L = 0x28;
  const int LIS3MDL_STATUS_REG = 0x27;

  int fd_;
};
