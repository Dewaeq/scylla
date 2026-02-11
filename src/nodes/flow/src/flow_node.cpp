#include "flow/flow_node.hpp"

#include "common/lcm_node.hpp"
#include "scylla_msgs/flow_t.hpp"

FlowNode::FlowNode() : LcmNode("flow_node") {
  if (driver.begin() != 0) {
    error("failed to init flow driver!");
    exit(-1);
  }

  info("flow driver initialised");
}

void FlowNode::update() {
  static int16_t dx, dy;
  static scylla_msgs::flow_t msg;

  driver.read_motion_burst(dx, dy);

  msg.timestamp = now_ns();
  msg.dx = dx;
  msg.dy = dy;
  publish("sensors/flow", msg);
}
