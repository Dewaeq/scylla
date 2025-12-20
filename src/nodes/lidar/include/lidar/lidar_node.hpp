#pragma once

#include "common/lcm_node.hpp"
#include "rplidar.h"

using namespace rp::standalone::rplidar;

class LidarNode : public LcmNode {
public:
  LidarNode();

private:
  RPlidarDriver *driver_;
};
