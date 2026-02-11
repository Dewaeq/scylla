#include "flow/flow_driver.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <ios>
#include <iostream>
#include <linux/spi/spidev.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <wiringPi.h>

FlowDriver::FlowDriver() {}

int FlowDriver::begin() {
  if (wiringPiSetupGpio() != 0) {
    std::cerr << "wiringPi GPIO setup failed" << std::endl;
    return -1;
  }

  std::string dev_path = "/dev/spidev0." + std::to_string(SPI_CHANNEL);
  fd_ = open(dev_path.c_str(), O_RDWR);
  if (fd_ < 0) {
    std::cerr << "SPI setup failed" << std::endl;
    return -1;
  }

  uint8_t mode = SPI_MODE_3;
  if (ioctl(fd_, SPI_IOC_WR_MODE, &mode) < 0) {
    std::cerr << "failed to set SPI Mode 3" << std::endl;
    return -1;
  }

  // Configure Speed
  uint32_t speed = SPI_SPEED;
  if (ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
    std::cerr << "failed to set SPI Speed" << std::endl;
    return -1;
  }

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  // start power up sequence
  delay(1);
  digitalWrite(CS_PIN, LOW);
  delay(1);
  digitalWrite(CS_PIN, HIGH);
  delay(1);

  write_register(REG_POWER_UP_RESET, 0x5a);
  delay(50);

  // read motion registers once
  read_register(REG_MOTION);
  read_register(REG_DELTA_X_L);
  read_register(REG_DELTA_X_H);
  read_register(REG_DELTA_Y_L);
  read_register(REG_DELTA_Y_H);
  delay(20);
  // finished power up sequence

  uint8_t pid = read_register(REG_PRODUCT_ID);
  if (pid != 0x49) {
    std::cout << "pid: 0x" << std::hex << (int)pid << std::endl;
    return -3;
  }

  if (init_registers() != 0) {
    std::cout << "failed to init registers" << std::endl;
    return -4;
  };

  return 0;
}

int FlowDriver::init_registers() {
  write_register(0x7F, 0x00);
  write_register(0x61, 0xAD);
  write_register(0x7F, 0x03);
  write_register(0x40, 0x00);
  write_register(0x7F, 0x05);
  write_register(0x41, 0xB3);
  write_register(0x43, 0xF1);
  write_register(0x45, 0x14);
  write_register(0x5B, 0x32);
  write_register(0x5F, 0x34);
  write_register(0x7B, 0x08);
  write_register(0x7F, 0x06);
  write_register(0x44, 0x1B);
  write_register(0x40, 0xBF);
  write_register(0x4E, 0x3F);
  write_register(0x7F, 0x08);
  write_register(0x65, 0x20);
  write_register(0x6A, 0x18);
  write_register(0x7F, 0x09);
  write_register(0x4F, 0xAF);
  write_register(0x5F, 0x40);
  write_register(0x48, 0x80);
  write_register(0x49, 0x80);
  write_register(0x57, 0x77);
  write_register(0x60, 0x78);
  write_register(0x61, 0x78);
  write_register(0x62, 0x08);
  write_register(0x63, 0x50);
  write_register(0x7F, 0x0A);
  write_register(0x45, 0x60);
  write_register(0x7F, 0x00);
  write_register(0x4D, 0x11);
  write_register(0x55, 0x80);
  write_register(0x74, 0x1F);
  write_register(0x75, 0x1F);
  write_register(0x4A, 0x78);
  write_register(0x4B, 0x78);
  write_register(0x44, 0x08);
  write_register(0x45, 0x50);
  write_register(0x64, 0xFF);
  write_register(0x65, 0x1F);
  write_register(0x7F, 0x14);
  write_register(0x65, 0x67);
  write_register(0x66, 0x08);
  write_register(0x63, 0x70);
  write_register(0x7F, 0x15);
  write_register(0x48, 0x48);
  write_register(0x7F, 0x07);
  write_register(0x41, 0x0D);
  write_register(0x43, 0x14);
  write_register(0x4B, 0x0E);
  write_register(0x45, 0x0F);
  write_register(0x44, 0x42);
  write_register(0x4C, 0x80);
  write_register(0x7F, 0x10);
  write_register(0x5B, 0x02);
  write_register(0x7F, 0x07);
  write_register(0x40, 0x41);
  write_register(0x70, 0x00);

  delayMicroseconds(10000);

  write_register(0x32, 0x44);
  write_register(0x7F, 0x07);
  write_register(0x40, 0x40);
  write_register(0x7F, 0x06);
  write_register(0x62, 0xF0);
  write_register(0x63, 0x00);
  write_register(0x7F, 0x0D);
  write_register(0x48, 0xC0);
  write_register(0x6F, 0xD5);
  write_register(0x7F, 0x00);
  write_register(0x5B, 0xA0);
  write_register(0x4E, 0xA8);
  write_register(0x5A, 0x50);
  write_register(0x40, 0x80);

  write_register(0x7F, 0x00);
  write_register(0x5A, 0x10);
  write_register(0x54, 0x00);
  return 0;

  // "Upon power-up of PMW3901MB, there are a number of registers to
  // configure in order to achieve optimum performance of the chip. These
  // registers are PixArt proprietary information, thus no additional
  // information is provided in this datasheet with regards to these
  // register’s descriptions."
  write_register(0x7F, 0x00);
  write_register(0x55, 0x01);
  write_register(0x50, 0x07);
  write_register(0x7F, 0x0E);

  bool success = false;
  for (int i = 0; i < 5; i++) {
    write_register(0x43, 0x10);
    delay(10);
    uint8_t val = read_register(0x47);
    std::cout << "read " << std::hex << (int)val << std::endl;
    // if (read_register(0x47) == 0x08) {
    if (val == 0x08) {
      success = true;
      break;
    }

    delay(10);
  }

  // if (!success)
  //   return -1;

  if (read_register(0x67) & 0x80)
    write_register(0x48, 0x04);
  else
    write_register(0x48, 0x02);

  write_register(0x7F, 0x00);
  write_register(0x51, 0x7B);
  write_register(0x50, 0x00);
  write_register(0x55, 0x00);
  write_register(0x7F, 0x0E);

  if (read_register(0x73) == 0x00) {
    uint8_t c1 = read_register(0x70);
    uint8_t c2 = read_register(0x71);

    write_register(0x7F, 0x00);
    write_register(0x61, 0xAD);
    write_register(0x51, 0x70);
    write_register(0x7F, 0x0E);
    write_register(0x70, std::min(0x3f, c1 <= 28 ? c1 + 14 : c1 + 11));
    write_register(0x71, (int)c2 * 45 / 100);
  }

  write_register(0x7F, 0x00);
  write_register(0x61, 0xAD);
  write_register(0x7F, 0x03);
  write_register(0x40, 0x00);
  write_register(0x7F, 0x05);
  write_register(0x41, 0xB3);
  write_register(0x43, 0xF1);
  write_register(0x45, 0x14);
  write_register(0x5B, 0x32);
  write_register(0x5F, 0x34);
  write_register(0x7B, 0x08);
  write_register(0x7F, 0x06);
  write_register(0x44, 0x1B);
  write_register(0x40, 0xBF);
  write_register(0x4E, 0x3F);
  write_register(0x7F, 0x08);
  write_register(0x65, 0x20);
  write_register(0x6A, 0x18);
  write_register(0x7F, 0x09);
  write_register(0x4F, 0xAF);
  write_register(0x5F, 0x40);
  write_register(0x48, 0x80);
  write_register(0x49, 0x80);
  write_register(0x57, 0x77);
  write_register(0x60, 0x78);
  write_register(0x61, 0x78);
  write_register(0x62, 0x08);
  write_register(0x63, 0x50);
  write_register(0x7F, 0x0A);
  write_register(0x45, 0x60);
  write_register(0x7F, 0x00);
  write_register(0x4D, 0x11);
  write_register(0x55, 0x80);
  write_register(0x74, 0x1F);
  write_register(0x75, 0x1F);
  write_register(0x4A, 0x78);
  write_register(0x4B, 0x78);
  write_register(0x44, 0x08);
  write_register(0x45, 0x50);
  write_register(0x64, 0xFF);
  write_register(0x65, 0x1F);
  write_register(0x7F, 0x14);
  write_register(0x65, 0x67);
  write_register(0x66, 0x08);
  write_register(0x63, 0x70);
  write_register(0x7F, 0x15);
  write_register(0x48, 0x48);
  write_register(0x7F, 0x07);
  write_register(0x41, 0x0D);
  write_register(0x43, 0x14);
  write_register(0x4B, 0x0E);
  write_register(0x45, 0x0F);
  write_register(0x44, 0x42);
  write_register(0x4C, 0x80);
  write_register(0x7F, 0x10);
  write_register(0x5B, 0x02);
  write_register(0x7F, 0x07);
  write_register(0x40, 0x41);
  write_register(0x70, 0x00);

  delay(20);

  write_register(0x32, 0x44);
  write_register(0x7F, 0x07);
  write_register(0x40, 0x40);
  write_register(0x7F, 0x06);
  write_register(0x62, 0xF0);
  write_register(0x63, 0x00);
  write_register(0x7F, 0x0D);
  write_register(0x48, 0xC0);
  write_register(0x6F, 0xD5);
  write_register(0x7F, 0x00);
  write_register(0x5B, 0xA0);
  write_register(0x4E, 0xA8);
  write_register(0x5A, 0x50);
  write_register(0x40, 0x80);
  delayMicroseconds(50);

  write_register(0x7F, 0x00);
  write_register(0x5A, 0x10);
  write_register(0x54, 0x00);

  return 0;
}

void FlowDriver::read_motion(int16_t &delta_x, int16_t &delta_y) {
  uint8_t motion = read_register(REG_MOTION);

  if (motion & 0x80) {
    uint8_t xl = read_register(REG_DELTA_X_L);
    uint8_t xh = read_register(REG_DELTA_X_H);
    uint8_t yl = read_register(REG_DELTA_Y_L);
    uint8_t yh = read_register(REG_DELTA_Y_H);

    // Combine Low and High bytes (16-bit signed)
    delta_x = (int16_t)((xh << 8) | xl);
    delta_y = (int16_t)((yh << 8) | yl);
  } else {
    delta_x = 0;
    delta_y = 0;
  }
}

uint8_t FlowDriver::read_register(uint8_t reg) {
  // from datasheet:
  // "The first byte contains the address (seven bits) and has '0' as its MSB to
  // indicate data direction. The second byte contains the data"

  struct spi_ioc_transfer tr[2];
  memset(tr, 0, sizeof(tr));

  uint8_t rx_data = 0;

  tr[0].tx_buf = (uint64_t)&reg;
  tr[0].len = 1;
  tr[0].speed_hz = SPI_SPEED;
  tr[0].delay_usecs = 50;
  tr[0].cs_change = 0;

  tr[1].rx_buf = (uint64_t)&rx_data;
  tr[1].len = 1;
  tr[1].speed_hz = SPI_SPEED;
  tr[1].delay_usecs = 50;
  tr[1].cs_change = 1;

  digitalWrite(CS_PIN, LOW);
  delayMicroseconds(50);
  int ret = ioctl(fd_, SPI_IOC_MESSAGE(2), tr);
  delayMicroseconds(50);
  digitalWrite(CS_PIN, HIGH);

  if (ret < 1) {
    std::cerr << "SPI read error" << std::endl;
    return 0;
  }

  delayMicroseconds(200);
  return rx_data;
}

void FlowDriver::write_register(uint8_t reg, uint8_t data) {
  // from datasheet:
  // "The first byte contains the address (seven bits) and has '1' as its MSB to
  // indicate data direction. The second byte contains the data"

  reg |= 0x80u;
  uint8_t tx_buf[2] = {reg, data};

  struct spi_ioc_transfer tr[2];
  memset(&tr, 0, sizeof(tr));

  tr[0].tx_buf = (uint64_t)&reg;
  tr[0].len = 1;
  tr[0].speed_hz = SPI_SPEED;
  tr[0].delay_usecs = 50;
  tr[0].cs_change = 0;

  tr[1].rx_buf = (uint64_t)&data;
  tr[1].len = 1;
  tr[1].speed_hz = SPI_SPEED;
  tr[1].delay_usecs = 50;
  tr[1].cs_change = 1;

  digitalWrite(CS_PIN, LOW);
  delayMicroseconds(50);
  int ret = ioctl(fd_, SPI_IOC_MESSAGE(2), tr);
  delayMicroseconds(50);
  digitalWrite(CS_PIN, HIGH);

  delayMicroseconds(200);
}
