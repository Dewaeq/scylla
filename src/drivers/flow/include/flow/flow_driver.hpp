#pragma once

#include <cstdint>
class FlowDriver {
public:
  FlowDriver();

  int begin();

private:
  const int SPI_CHANNEL = 0;
  const int SPI_SPEED = 2000000; // 2MHz
  const int CS_PIN = 10;

  const uint8_t REG_PRODUCT_ID = 0x00;
  const uint8_t REG_REVISION_ID = 0x01;
  const uint8_t REG_POWER_UP_RESET = 0x3a;
  const uint8_t REG_MOTION = 0x02;
  const uint8_t REG_DELTA_X_L = 0x03;
  const uint8_t REG_DELTA_X_H = 0x04;
  const uint8_t REG_DELTA_Y_L = 0x05;
  const uint8_t REG_DELTA_Y_H = 0x06;

  uint8_t read_register(uint8_t reg);

  void write_register(uint8_t reg, uint8_t data);
};
