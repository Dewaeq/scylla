#pragma once

#include <functional>
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
  void subscribe(const std::string &channel, void (T::*handler)(const M *),
                 T *target) {

    lcm::LCM::HandlerFunction<M> wrapper =
        [target, handler](const lcm::ReceiveBuffer *rbuf,
                          const std::string &chan,
                          const M *msg) { (target->*handler)(msg); };

    lcm_.subscribe(channel, wrapper);
  }

  // usage: subscribe("CHANNEL", <callback>);
  template <typename M>
  void subscribe(const std::string &channel,
                 std::function<void(const M *)> callback) {
    lcm::LCM::HandlerFunction<M> wrapper =
        [callback](const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                   const M *msg) { callback(msg); };
    lcm_.subscribe(channel, wrapper);
  }

  // current time in seconds
  double now();

  // current time in nanoseconds
  int64_t now_ns();

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
