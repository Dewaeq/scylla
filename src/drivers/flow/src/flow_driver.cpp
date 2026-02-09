#include "flow/flow_driver.hpp"

#include <cstdint>
#include <ios>
#include <iostream>
#include <wiringPi.h>
#include <wiringPiSPI.h>

FlowDriver::FlowDriver() {}

int FlowDriver::begin() {
  if (wiringPiSetup() != -1) {
    std::cerr << "wiringPi setup failed" << std::endl;
    return -1;
  }

  if (wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) != 0) {
    std::cerr << "wiringPi SPI setup failed" << std::endl;
    return -2;
  }

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  delay(1);

  uint8_t pid = read_register(0x00);
  std::cout << "pid: 0x" << std::hex << pid << std::endl;

  return 0;
}

uint8_t FlowDriver::read_register(uint8_t reg) {
  // from datasheet:
  // "The first byte contains the address (seven bits) and has '0' as its MSB to
  // indicate data direction. The second byte contains the data"

  uint8_t buffer[2];
  buffer[0] = reg & 0x7f;

  digitalWrite(CS_PIN, LOW);
  wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2);
  digitalWrite(CS_PIN, HIGH);

  return buffer[1];
}

void FlowDriver::write_register(uint8_t reg, uint8_t data) {
  // from datasheet:
  // "The first byte contains the address (seven bits) and has '1' as its MSB to
  // indicate data direction. The second byte contains the data"

  uint8_t buffer[2];
  buffer[0] = reg | 0x80;
  buffer[1] = data;

  digitalWrite(CS_PIN, LOW);
  wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2);
  digitalWrite(CS_PIN, HIGH);
}
