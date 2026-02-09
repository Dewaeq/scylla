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

  uint8_t read_register(uint8_t reg);

  void write_register(uint8_t reg, uint8_t data);
};
