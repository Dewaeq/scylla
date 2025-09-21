#ifndef PROBABILITY_GRID_H
#define PROBABILITY_GRID_H

#include <eigen3/Eigen/Dense>
#include <vector>

class ProbabilityGrid {
public:
  ProbabilityGrid(int width, int height, float resolution,
                  Eigen::Isometry2f world_transform);

  void update(const Eigen::Isometry2f &robot_transform,
              const std::vector<Eigen::Vector2f> &observations);
  void save_as_pgm(const std::string &file_name);

private:
  int width;
  int height;
  // cell size in mm
  float resolution;
  std::vector<float> grid;
  Eigen::Vector2f origin;
  // position in the world frame
  Eigen::Isometry2f grid_trans;

  float at(int x, int y) const;
  void add(const Eigen::Vector2i &cell, float val);
  bool in_bounds(const Eigen::Vector2i &cell) const;
  Eigen::Vector2i world_to_cell(const Eigen::Vector2f world_pos);
  std::vector<Eigen::Vector2i> get_ray_cells(const Eigen::Vector2i &cell0,
                                             const Eigen::Vector2i &cell1);
};

#endif
