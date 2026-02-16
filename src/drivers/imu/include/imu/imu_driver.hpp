#pragma once

struct ImuData {
  float ax, ay, az;
  float gx, gy, gz;
};

struct MagData {
  float mx, my, mz;
};

class ImuDriver {
public:
  ImuDriver();

  int begin();

  bool has_imu_data();
  bool has_mag_data();

  ImuData read_imu();
  MagData read_mag();

private:
  const int POWER_PIN = 4;
  // IMU:
  const int LSM6DSOX_ADDR = 0x6A;
  const int LSM6DSOX_WHO_AM_I = 0x0F;
  const int LSM6DSOX_CTRL1_XL = 0x10;
  const int LSM6DSOX_CTRL2_G = 0x11;
  const int LSM6DSOX_OUTX_L_G = 0x22;
  const int LSM6DSOX_OUTX_L_A = 0x28;
  const int LSM6DSOX_STATUS_REG = 0x1E;

  // Magnetometer:
  const int LIS3MDL_ADDR = 0x1C;
  const int LIS3MDL_WHO_AM_I = 0x0F;
  const int LIS3MDL_CTRL_REG1 = 0x20;
  const int LIS3MDL_CTRL_REG2 = 0x21;
  const int LIS3MDL_CTRL_REG3 = 0x22;
  const int LIS3MDL_OUT_X_L = 0x28;
  const int LIS3MDL_STATUS_REG = 0x27;

  int fd_imu_;
  int fd_mag_;
};
