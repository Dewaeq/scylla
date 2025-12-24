#include "control/control_node.hpp"

int main(int argc, char *argv[]) {
  ControlNode node;
  node.spin_hz(10);

  return 0;
}
