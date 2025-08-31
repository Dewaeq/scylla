#include <iostream>
#include <lcm/lcm-cpp.hpp>
#include <unistd.h>

#include "messages/SteerCommand.hpp"

int main() {
  lcm::LCM lcm;
  if (!lcm.good()) {
    std::cout << "[ERROR] failed to init lcm" << std::endl;
    return -1;
  }

  while (1) {
    messages::SteerCommand cmd;
    cmd.speed = 0;
    cmd.turn = (rand() % 3) - 1;

    lcm.publish("steer_command", &cmd);
    std::cout << "publishing command" << std::endl;
    sleep(1);
  }

  return 0;
}
