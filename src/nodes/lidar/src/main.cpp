#include "lidar/lidar_node.hpp"

int main(int argc, char *argv[]) {
  LidarNode node;
  node.spin_hz(10);

  return 0;
}
