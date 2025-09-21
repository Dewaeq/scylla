#include <glog/logging.h>
#include <lcm/lcm-cpp.hpp>

#include "slam_frontend.hpp"

int main(int argc, char *argv[]) {
  google::InitGoogleLogging(argv[0]);

  lcm::LCM lcm;
  if (!lcm.good())
    return -1;

  SlamFrontend frontend;
  lcm.subscribe("filtered_scan", &SlamFrontend::handleScan, &frontend);

  while (lcm.handle() == 0)
    ;

  return 0;
}
