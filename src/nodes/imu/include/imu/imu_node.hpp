#pragma once

#include "common/lcm_node.hpp"
#include "imu/imu_driver.hpp"

class ImuNode : public LcmNode {
public:
  ImuNode();

  virtual void update() override;

private:
  ImuDriver driver_;
};
