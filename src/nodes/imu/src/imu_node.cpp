#include "imu/imu_node.hpp"
#include "common/lcm_node.hpp"
#include <sstream>

ImuNode::ImuNode() : LcmNode("imu_node") {
  if (driver_.begin() != 0) {
    error("failed to init imu driver!");
    exit(-1);
  }

  info("imu driver initialised");
}

void ImuNode::update() {
  const auto data = driver_.read_data();
  std::stringstream s;
  s << "[data]: "
    << "\tax: " << data.ax << "\tay: " << data.ay << "\taz: " << data.az
    << std::endl
    << "\tgx: " << data.gx << "\tgy: " << data.gy << "\tgz: " << data.gz
    << std::endl
    << "\tmx: " << data.mx << "\tmy: " << data.my << "\tmz: " << data.mz;
  info(s.str());
}
