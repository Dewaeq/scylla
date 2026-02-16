#include "imu/imu_node.hpp"
int main(int argc, char *argv[]) {
  ImuNode node;
  node.spin_hz(2);

  return 0;
}
