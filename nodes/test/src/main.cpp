#include <eigen3/Eigen/Dense>
#include <iostream>
#include <lcm/lcm-cpp.hpp>
#include <sstream>

#include "common/probability_grid.hpp"
#include "messages/LaserScan.hpp"

using namespace lcm;

class Handler {
public:
  Handler(ProbabilityGrid &g) : grid(g) {};
  void handleMessage(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                     const messages::LaserScan *msg) {
    std::vector<Eigen::Vector2f> observations;
    for (const auto &pt : msg->points) {
      observations.emplace_back(pt.x, pt.y);
    }

    grid.update(Eigen::Isometry2f::Identity(), observations);

    // std::stringstream sstream;
    // sstream << "scans/" << "scan_" << msg->timestamp << ".pgm";
    // std::string file_name = sstream.str();
    //
    // std::cout << "[INFO] storing map to: " << file_name << std::endl;
    //
    // grid.save_as_pgm(file_name);
  }
  ProbabilityGrid &grid;
};

int main(int argc, char *argv[]) {
  std::cout << "hi there" << std::endl;

  ProbabilityGrid grid(400, 400, 20, Eigen::Isometry2f::Identity());

  lcm::LCM lcm;
  if (!lcm.good())
    return -1;

  Handler handler(grid);
  lcm.subscribe("filtered_scan", &Handler::handleMessage, &handler);

  while (lcm.handle() == 0)
    ;

  return 0;
}
