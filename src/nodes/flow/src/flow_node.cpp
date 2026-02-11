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
  int16_t dx, dy;

  if (driver.read_motion(dx, dy)) {
    scylla_msgs::flow_t msg;
    msg.timestamp = now_ns();
    msg.dx = dx;
    msg.dy = dy;

    publish("sensors/flow", msg);
  };
}
