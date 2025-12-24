#pragma once

#include "common/lcm_node.hpp"
#include "motor/motor_driver.hpp"
#include "scylla_msgs/drive_command_t.hpp"
#include <lcm/lcm-cpp.hpp>

class ControlNode : public LcmNode {
public:
  ControlNode();

  virtual void update() override;

private:
  void handle_command(const scylla_msgs::drive_command_t *msg);

  MotorDriver driver_;
  int64_t last_cmd_time_ms_ = 0;
};
