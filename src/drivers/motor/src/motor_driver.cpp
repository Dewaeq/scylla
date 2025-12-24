#include "motor/motor_driver.hpp"

#include "wiringPi.h"
#include <algorithm>
#include <iostream>

MotorDriver::MotorDriver() {
  if (wiringPiSetupGpio() != 0) {
    std::cerr << "wiringPi setup failed" << std::endl;
    exit(-1);
  }

  pinMode(PIN_M1, OUTPUT);
  pinMode(PIN_M2, OUTPUT);
  pinMode(PIN_M_PWM, PWM_OUTPUT);

  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);
  pinMode(PIN_S_PWM, PWM_OUTPUT);

  pwmSetRange(100);
}

void MotorDriver::set_throttle(double val) {
  val = std::clamp(val, -1.0, 1.0);

  bool forward = val > 0;
  int pwm = std::abs(val) * 100;

  digitalWrite(PIN_M1, forward);
  digitalWrite(PIN_M2, !forward);
  pwmWrite(PIN_M_PWM, pwm);
}

void MotorDriver::set_steering(double val) {
  if (val < -0.5) {
    // left
    digitalWrite(PIN_S1, 1);
    digitalWrite(PIN_S2, 0);
    pwmWrite(PIN_S_PWM, 100);
  } else if (val > 0.5) {
    // right
    digitalWrite(PIN_S1, 0);
    digitalWrite(PIN_S2, 1);
    pwmWrite(PIN_S_PWM, 100);
  } else {
    // center
    digitalWrite(PIN_S1, 0);
    digitalWrite(PIN_S2, 0);
    pwmWrite(PIN_S_PWM, 0);
  }
}
