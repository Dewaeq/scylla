#ifndef SCAN_FILTER_H
#define SCAN_FILTER_H

#include <Eigen/Dense>
#include <map>

#include "messages/LaserScan.hpp"

class ScanFilter {
public:
  void filter(messages::LaserScan &scan);

private:
  std::map<std::pair<int, int>, std::pair<Eigen::Vector2f, int>> grid;
};

#endif
