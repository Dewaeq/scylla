#include "flow/flow_node.hpp"

int main(int argc, char *argv[]) {
  FlowNode node;
  node.spin_hz(10);

  return 0;
}
