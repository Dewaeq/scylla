#pragma once

class MotorDriver {
public:
  MotorDriver();

  void set_throttle(double val);
  void set_steering(double val);

private:
  const int PIN_M1 = 5;
  const int PIN_M2 = 6;
  const int PIN_M_PWM = 13;

  const int PIN_S1 = 23;
  const int PIN_S2 = 24;
  const int PIN_S_PWM = 18;
};
