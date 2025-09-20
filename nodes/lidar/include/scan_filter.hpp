#ifndef SCAN_FILTER_H
#define SCAN_FILTER_H

#include <eigen3/Eigen/Dense>
#include <map>

#include "messages/LaserScan.hpp"

struct VoxelStats {
  Eigen::Vector2f acc_pos = Eigen::Vector2f::Zero();
  float count = 0.f;

  Eigen::Vector2f avg_pos() const { return this->acc_pos / this->count; }
};

class ScanFilter {
public:
  void filter(messages::LaserScan &scan);

private:
  std::map<std::pair<int, int>, VoxelStats> grid;
};

#endif
