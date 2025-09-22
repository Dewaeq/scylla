#include <KDTree.hpp>
#include <eigen3/Eigen/Dense>
#include <glog/logging.h>
#include <vector>

#include "common/icp.hpp"

using namespace Eigen;

// https://learnopencv.com/iterative-closest-point-icp-explained/
namespace icp {
Eigen::Isometry2f align_scans(const Scan &reference_points,
                              const Scan &current_scan, Pose initial_estimate,
                              int max_iterations) {
  Pose current_pose = initial_estimate;

  pointVec tree_points;
  for (const auto &pt : reference_points) {
    tree_points.push_back(std::vector<double>({pt.x(), pt.y()}));
  }

  KDTree tree(tree_points);
  // store pairs of {reference_point, transformed_scan_point}
  std::vector<std::pair<Point, Point>> associations;

  for (int i = 0; i < max_iterations; i++) {
    for (const auto &pt : current_scan) {
      Point transformed = current_pose * pt;
      point_t tree_point({transformed.x(), transformed.y()});
      point_t reference_point_t = tree.nearest_point(tree_point);
      Point reference_point(reference_point_t[0], reference_point_t[1]);
      associations.push_back({reference_point, transformed});
    }

    Isometry2f delta = calculate_svd_transform(associations);
    current_pose = delta * current_pose;

    // float err = (delta.matrix() - Isometry2f::Identity().matrix()).norm();
    // if (err < ...) break;
  }

  return current_pose;
}

Isometry2f calculate_svd_transform(std::vector<std::pair<Point, Point>> pairs) {
  Point ref_centroid = Point::Zero();
  Point scan_centroid = Point::Zero();

  for (const auto &[reference_point, scan_point] : pairs) {
    ref_centroid += reference_point;
    scan_centroid += scan_point;
  }

  ref_centroid /= pairs.size();
  scan_centroid /= pairs.size();

  Matrix2f H = Matrix2f::Zero();
  for (const auto &[reference_point, scan_point] : pairs) {
    H += (scan_point - scan_centroid) *
         (reference_point - ref_centroid).transpose();
  }

  JacobiSVD svd(H, Eigen::ComputeFullU | Eigen::ComputeFullV);
  Matrix2f U = svd.matrixU();
  Matrix2f V = svd.matrixV();
  Matrix2f R = V * U.transpose();

  if (R.determinant() < 0) {
    V.col(1) *= -1;
    R = V * U.transpose();
  }

  Vector2f t = ref_centroid - R * scan_centroid;

  // Construct the Isometry2f from the calculated rotation (R) and translation
  // (t)
  Isometry2f result = Isometry2f::Identity();
  result.linear() = R;
  result.translation() = t;

  return result;
}
} // namespace icp
