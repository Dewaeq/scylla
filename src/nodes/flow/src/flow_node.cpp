#include "flow/flow_node.hpp"
#include "common/lcm_node.hpp"
#include <cstdlib>

FlowNode::FlowNode() : LcmNode("flow_node") {
  if (driver.begin() != 0) {
    error("failed to init flow driver!");
    exit(-1);
  }

  info("flow driver initialised");
}
