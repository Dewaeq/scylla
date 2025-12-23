#pragma once

#include <iostream>
#include <lcm/lcm-cpp.hpp>
#include <ostream>

class LcmNode {
public:
  LcmNode(const std::string &node_name);

  bool ok() const;

  // mode 1: event driven
  // blocks forever, waking up only when a message arrives
  void spin_blocking();
  // mode 2: fixed frequency loop:
  // wakes up [hz] times per second
  void spin_hz(int hz);
  void spin_once();

  virtual void update() {};

  template <typename MessageType>
  void publish(const std::string &channel, const MessageType &msg) {
    lcm_.publish(channel, &msg);
  }

  // usage: subscribe("CHANNEL", &MyClass::callback, this);
  template <typename M, typename T>
  void subscribe(const std::string &channel,
                 void (T::*handler)(const lcm::ReceiveBuffer *,
                                    const std::string &, const M *),
                 T *target) {
    lcm_.subscribe(channel, handler, target);
  }

protected:
  void info(const std::string &s);
  void warn(const std::string &s);
  void error(const std::string &s);

private:
  void log(std::ostream &os, const std::string &s);

  lcm::LCM lcm_;
  bool ok_;
  std::string node_name_;
};
