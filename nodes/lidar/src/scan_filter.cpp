#include <eigen3/Eigen/Dense>
#include <map>

#include "scan_filter.hpp"

// min distance in mm
#define MIN_DIST 80
// max distance in mm
#define MAX_DIST 10000
// voxel size in mm
#define VOXEL_SIZE 50.0

using namespace Eigen;

void ScanFilter::filter(messages::LaserScan &scan) {
  for (const auto &point : scan.points) {
    const Vector2f pos(point.x, point.y);
    const auto dist = pos.norm();

    if (dist < MIN_DIST || dist > MAX_DIST)
      continue;

    const int x = std::floor(point.x / VOXEL_SIZE);
    const int y = std::floor(point.y / VOXEL_SIZE);
    const auto key = std::make_pair(x, y);

    grid[key].acc_pos += pos;
    grid[key].count += 1;
  }

  int count = 0;
  for (const auto &[cell, stats] : grid) {
    const Vector2f avg = stats.avg_pos();
    scan.points[count].x = avg.x();
    scan.points[count].y = avg.y();
    count++;
  }

  scan.num_points = count;
  grid.clear();
}
