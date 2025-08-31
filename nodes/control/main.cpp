#include <iostream>
#include <lcm/lcm-cpp.hpp>
#include <wiringPi.h>

#include "messages/SteerCommand.hpp"

#define MOTOR_1 5
#define MOTOR_2 6
#define MOTOR_SPEED 13

#define STEER_1 23
#define STEER_2 24
#define STEER_SPEED 18

class Handler {
public:
  void handleMessage(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                     const messages::SteerCommand *msg) {
    if (msg->speed < -255 || msg->speed > 255) {
      std::cout << "[WARNING] SteerCommand::speed is out of range: "
                << msg->speed << std::endl;
      return;
    }
    if (msg->turn < -1 || msg->turn > 1) {
      std::cout << "[WARNING] SteerCommand::turn is out of range: "
                << msg->speed << std::endl;
      return;
    }

    bool forward = msg->speed > 0;
    digitalWrite(MOTOR_1, forward);
    digitalWrite(MOTOR_2, !forward);
    pwmWrite(MOTOR_SPEED, abs(msg->speed));

    std::cout << "[INFO] applying steer command" << std::endl;

    switch (msg->turn) {
    case -1:
    case 1:
      digitalWrite(STEER_1, msg->turn == -1);
      digitalWrite(STEER_2, msg->turn == 1);
      pwmWrite(STEER_SPEED, 250);
      break;
    default:
      pwmWrite(STEER_SPEED, 0);
      break;
    }
  }
};

int main() {
  int err = wiringPiSetupGpio();
  if (err) {
    std::cout << "[ERROR] failed to setup gpio" << std::endl;
    return -1;
  }

  pinMode(MOTOR_SPEED, PWM_OUTPUT);
  pinMode(STEER_SPEED, PWM_OUTPUT);
  pinMode(MOTOR_1, OUTPUT);
  pinMode(MOTOR_2, OUTPUT);
  pinMode(STEER_1, OUTPUT);
  pinMode(STEER_2, OUTPUT);

  lcm::LCM lcm;
  if (!lcm.good()) {
    std::cout << "[ERROR] failed to init lcm" << std::endl;
    return -1;
  }

  Handler handler;
  lcm.subscribe("steer_command", &Handler::handleMessage, &handler);

  while (lcm.handle() == 0)
    ;

  return 0;
}
