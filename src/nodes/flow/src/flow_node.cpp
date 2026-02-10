#include "flow/flow_node.hpp"
#include "common/lcm_node.hpp"
#include <cstdlib>
#include <sstream>

FlowNode::FlowNode() : LcmNode("flow_node") {
  if (driver.begin() != 0) {
    error("failed to init flow driver!");
    exit(-1);
  }

  info("flow driver initialised");
}

void FlowNode::update() {
  int16_t dx, dy;
  driver.read_motion(dx, dy);

  std::stringstream ss;
  ss << "dx: " << dx << ", dy: " << dy;
  info(ss.str());
}
