#ifndef LIDAR_H
#define LIDAR_H

#include <string>

#include "rplidar.h"
#include "sl_lidar.h"
#include "sl_lidar_driver.h"

class Lidar {
public:
  Lidar(const std::string &device);
  bool connect();

private:
  sl::IChannel *channel;
  sl::ILidarDriver *driver;
};

#endif
