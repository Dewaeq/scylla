#include "common/lcm_node.hpp"
#include <chrono>
#include <thread>

LcmNode::LcmNode(const std::string &node_name)
    : node_name_(node_name), ok_(false) {

  if (!lcm_.good()) {
    error("failed to init lcm");
    exit(-1);
  }

  ok_ = true;
}

bool LcmNode::ok() const { return ok_; }

void LcmNode::spin_blocking() {
  while (lcm_.handle() == 0)
    ;
}

void LcmNode::spin_hz(int hz) {
  auto time_per_loop = std::chrono::milliseconds(1000 / hz);

  while (true) {
    auto start_time = std::chrono::steady_clock::now();

    int safety_count = 50;
    while (lcm_.handleTimeout(0) > 0 && safety_count--)
      ;

    update();

    std::this_thread::sleep_until(start_time + time_per_loop);
  }
}

void LcmNode::spin_once() { lcm_.handleTimeout(0); }

void LcmNode::info(const std::string &s) { log(std::cout, "[INFO]: " + s); }
void LcmNode::warn(const std::string &s) { log(std::cerr, "[WARN]: " + s); }
void LcmNode::error(const std::string &s) { log(std::cerr, "[ERROR]: " + s); }

void LcmNode::log(std::ostream &os, const std::string &s) {
  os << "[" << node_name_ << "]: " << s << std::endl;
}

double LcmNode::now() {
  auto time = std::chrono::steady_clock::now();
  auto sec =
      std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch());

  return std::chrono::duration<double>(sec).count();
}

int64_t LcmNode::now_ns() {
  auto time = std::chrono::steady_clock::now();
  auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
      time.time_since_epoch());

  return static_cast<int64_t>(ns.count());
}
