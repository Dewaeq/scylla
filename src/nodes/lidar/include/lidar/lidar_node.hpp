#pragma once

#include "common/lcm_node.hpp"
#include "rplidar.h"

using namespace sl;

class LidarNode : public LcmNode {
public:
  LidarNode();
  ~LidarNode();

  virtual void update() override;

private:
  IChannel *channel_;
  ILidarDriver *driver_;
};
