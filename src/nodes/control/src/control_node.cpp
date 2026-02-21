#include "control/control_node.hpp"
#include "scylla_msgs/drive_command_t.hpp"

ControlNode::ControlNode() : LcmNode("control_node") {
  driver_.set_steering(0);
  driver_.set_throttle(0);

  subscribe("drive_command", &ControlNode::handle_command, this);
}

void ControlNode::handle_command(const scylla_msgs::drive_command_t *msg) {
  const auto dt = now() - (double)msg->timestamp / 1e9;
  if (dt > 0.1) {
    warn("ignoring packet, dt=" + std::to_string(dt));
    return;
  }

  driver_.set_throttle(msg->throttle);
  driver_.set_steering(msg->steering);

  last_cmd_time_ = now();
}

void ControlNode::update() {
  // SAFETY CHECK:
  // stop if no command for 200ms
  if ((now() - last_cmd_time_) > 0.2) {
    driver_.set_steering(0);
    driver_.set_throttle(0);
  }
}
