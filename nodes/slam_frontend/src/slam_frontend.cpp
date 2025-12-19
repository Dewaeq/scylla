#include <chrono>
#include <eigen3/Eigen/Dense>
#include <glog/logging.h>
#include <sstream>

#include "common/icp.hpp"
#include "slam_frontend.hpp"

using namespace Eigen;
using namespace std::chrono;

SlamFrontend::SlamFrontend() : current_pose(Isometry2f::Identity()) {
  submap = ProbabilityGrid(SUBMAP_SIZE, SUBMAP_SIZE, SUBMAP_RES,
                           Eigen::Isometry2f::Identity());
}

void SlamFrontend::handleScan(const lcm::ReceiveBuffer *rbuf,
                              const std::string &chan,
                              const messages::LaserScan *msg) {
  auto t0 = steady_clock::now();

  Isometry2f initial_estimate = current_pose;
  Scan reference_points =
      submap.get_points_box(initial_estimate.translation(), 10000.0);
  Scan current_scan;
  current_scan.reserve(msg->num_points);
  for (const auto &pt : msg->points) {
    current_scan.emplace_back(pt.x, pt.y);
  }

  if (reference_points.size() < 20) {
    LOG(WARNING) << "not enough points to align scans";
  } else {
    Pose new_pose =
        icp::align_scans(reference_points, current_scan, initial_estimate, 25);
    LOG(INFO) << "estimated new pose: " << "(" << new_pose.translation().x()
              << "," << new_pose.translation().y() << ")";
    // current_pose = new_pose;
  }

  submap.update(current_pose, current_scan);
  auto t1 = steady_clock::now();

  duration<double> elapsed = t1 - t0;
  LOG(INFO) << "scan aligment took " << elapsed.count() << " seconds";

  std::stringstream ss;
  ss << "scans/" << msg->timestamp << ".pgm";
  submap.save_as_pgm(ss.str());
}
