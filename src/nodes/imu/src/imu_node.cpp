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
  if (driver_.has_imu_data()) {
    const auto data = driver_.read_imu();
    std::stringstream s;
    s << "[imu data]: "
      << "\tax: " << data.ax << "\tay: " << data.ay << "\taz: " << data.az
      << std::endl
      << "\tgx: " << data.gx << "\tgy: " << data.gy << "\tgz: " << data.gz;
    // info(s.str());
  }
  if (driver_.has_mag_data()) {
    const auto data = driver_.read_mag();
    std::stringstream s;
    s << "[mag data]: "
      << "\tmx: " << data.mx << "\tmy: " << data.my << "\tmz: " << data.mz;
    info(s.str());
  }
}
