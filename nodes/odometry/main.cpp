#include <chrono>
#include <iostream>
#include <optional>

#include <Eigen/Dense>
#include <lcm/lcm-cpp.hpp>

#include "messages/SteerCommand.hpp"

using namespace Eigen;

class OdometryTracker {
private:
  Vector2f estimate = Vector2f::Zero();
  std::optional<messages::SteerCommand> last_command;
  std::chrono::steady_clock::time_point last_update =
      std::chrono::steady_clock::now();

public:
  void handleMessage(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                     const messages::SteerCommand *msg) {
    this->last_command = *msg;
    this->update(true);
  }

  void update(bool force = false) {
    if (!this->last_command)
      return;

    const auto now = std::chrono::steady_clock::now();
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::duration<double>>(
            now - this->last_update);
    const double elapsed_sec = elapsed.count();

    if (!force && elapsed_sec < 0.05) {
      return;
    }

    this->last_update = now;
  }

  void publish() {}
};

int main() {
  lcm::LCM lcm;
  if (!lcm.good()) {
    std::cout << "[ERROR] failed to init lcm" << std::endl;
    return -1;
  }

  OdometryTracker tracker;
  lcm.subscribe("steer_command", &OdometryTracker::handleMessage, &tracker);

  while (lcm.handle() == 0) {
    tracker.update();
  }

  return 0;
}
