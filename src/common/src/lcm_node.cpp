#include "common/lcm_node.hpp"

LcmNode::LcmNode(const std::string &node_name)
    : node_name_(node_name), ok_(false) {

  if (!lcm_.good()) {
    error("failed to init lcm");
    exit(-1);
  }

  ok_ = true;
}

bool LcmNode::ok() const { return ok_; }

void LcmNode::spin() {
  while (lcm_.handle() == 0)
    ;
}

void LcmNode::spin_once() { lcm_.handleTimeout(0); }

void LcmNode::info(const std::string &s) { log(std::cout, "[INFO]: " + s); }
void LcmNode::warn(const std::string &s) { log(std::cerr, "[WARN]: " + s); }
void LcmNode::error(const std::string &s) { log(std::cerr, "[ERROR]: " + s); }

void LcmNode::log(std::ostream &os, const std::string &s) {
  os << "[" << node_name_ << "]: " << s << std::endl;
}
