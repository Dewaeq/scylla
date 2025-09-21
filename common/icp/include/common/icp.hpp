#ifndef ICP_H
#define ICP_H

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/src/Geometry/Transform.h>
#include <vector>

using Pose = Eigen::Isometry2f;
using Point = Eigen::Vector2f;
using Scan = std::vector<Point>;

namespace icp {
Eigen::Isometry2f align_scans(const Scan &reference_points,
                              const Scan &current_scan, Pose initial_estimate,
                              int max_iterations);

Eigen::Isometry2f calculate_svd_transform(std::vector<std::pair<Point, Point>>);
} // namespace icp

#endif
