#pragma once

#include <cstdint>
class FlowDriver {
public:
  FlowDriver();

  int begin();
  bool read_motion(int16_t &delta_x, int16_t &delta_y);
  bool read_motion_burst(int16_t &delta_x, int16_t &delta_y);

private:
  const int SPI_CHANNEL = 0;
  const int SPI_SPEED = 2000000; // 2MHz
  const int CS_PIN = 25;
  const int MISO_PIN = 9;
  const int MOSI_PIN = 10;
  const int CLK_PIN = 11;

  const uint8_t REG_PRODUCT_ID = 0x00;
  const uint8_t REG_REVISION_ID = 0x01;
  const uint8_t REG_POWER_UP_RESET = 0x3A;
  const uint8_t REG_MOTION = 0x02;
  const uint8_t REG_MOTION_BURST = 0x16;
  const uint8_t REG_DELTA_X_L = 0x03;
  const uint8_t REG_DELTA_X_H = 0x04;
  const uint8_t REG_DELTA_Y_L = 0x05;
  const uint8_t REG_DELTA_Y_H = 0x06;

  uint8_t read_register(uint8_t reg);
  void write_register(uint8_t reg, uint8_t data);
  uint8_t spi_transfer(uint8_t data);

  int init_registers();
};
