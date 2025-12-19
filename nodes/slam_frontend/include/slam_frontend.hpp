#ifndef SLAM_FRONTEND_H
#define SLAM_FRONTEND_H

#include <eigen3/Eigen/Dense>
#include <lcm/lcm-cpp.hpp>

#include "common/probability_grid.hpp"
#include "messages/LaserScan.hpp"

#define SUBMAP_RES 50
#define SUBMAP_SIZE 400

class SlamFrontend {
public:
  SlamFrontend();
  void handleScan(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                  const messages::LaserScan *msg);

private:
  ProbabilityGrid submap;
  Eigen::Isometry2f current_pose;
};

#endif // !SLAM_FRONTEND_H
