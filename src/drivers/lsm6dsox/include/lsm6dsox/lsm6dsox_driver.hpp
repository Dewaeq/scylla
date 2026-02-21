#pragma once

struct Lsm6dsoxData {
  float ax, ay, az;
  float gx, gy, gz;
};

class Lsm6dsoxDriver {
public:
  Lsm6dsoxDriver();

  bool begin();
  bool has_data();
  bool read(Lsm6dsoxData &data);

private:
  const int LSM6DSOX_ADDR = 0x6A;
  const int LSM6DSOX_WHO_AM_I = 0x0F;
  const int LSM6DSOX_CTRL1_XL = 0x10;
  const int LSM6DSOX_CTRL2_G = 0x11;
  const int LSM6DSOX_OUTX_L_G = 0x22;
  const int LSM6DSOX_OUTX_L_A = 0x28;
  const int LSM6DSOX_STATUS_REG = 0x1E;

  int fd_;
};
