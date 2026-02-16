#include "imu_mag/imu_mag_node.hpp"

int main(int argc, char *argv[]) {
  ImuMagNode node;
  node.spin_hz(208);

  return 0;
}
