#pragma once

#include "common/lcm_node.hpp"
#include "flow/flow_driver.hpp"

class FlowNode : public LcmNode {
public:
  FlowNode();

  virtual void update() override;

private:
  FlowDriver driver;
};
