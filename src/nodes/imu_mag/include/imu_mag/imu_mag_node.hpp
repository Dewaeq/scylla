#pragma once

#include "common/lcm_node.hpp"
#include "lis3mdl/lis3mdl_driver.hpp"
#include "lsm6dsox/lsm6dsox_driver.hpp"

class ImuMagNode : public LcmNode {
public:
  ImuMagNode();

  virtual void update() override;

private:
  const int POWER_PIN = 4;

  Lsm6dsoxDriver imu_driver_;
  Lis3mdlDriver mag_driver_;
};
