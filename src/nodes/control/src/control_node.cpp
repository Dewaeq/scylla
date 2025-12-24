#include "control/control_node.hpp"
#include "scylla_msgs/drive_command_t.hpp"
#include <chrono>

ControlNode::ControlNode() : LcmNode("control_node") {
  driver_.set_steering(0);
  driver_.set_throttle(0);

  subscribe("drive_command", &ControlNode::handle_command, this);
}

void ControlNode::handle_command(const scylla_msgs::drive_command_t *msg) {
  driver_.set_throttle(msg->throttle);
  driver_.set_steering(msg->steering);

  auto now = std::chrono::steady_clock::now();
  last_cmd_time_ms_ = std::chrono::duration_cast<std::chrono::milliseconds>(
                          now.time_since_epoch())
                          .count();
}

void ControlNode::update() {
  auto now = std::chrono::steady_clock::now();
  int64_t current_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now.time_since_epoch())
                           .count();

  // SAFETY CHECK:
  // stop if no command for 500ms
  if ((current_ms - last_cmd_time_ms_) > 500) {
    driver_.set_steering(0);
    driver_.set_throttle(0);
  }
}
