#include <algorithm>
#include <eigen3/Eigen/Dense>
#include <fstream>

#include "common/probability_grid.hpp"

using namespace Eigen;

ProbabilityGrid::ProbabilityGrid(int width, int height, float resolution,
                                 Isometry2f grid_trans)
    : width(width), height(height), resolution(resolution),
      grid_trans(grid_trans) {
  grid.resize(width * height);
  origin.x() = -0.5 * width;
  origin.y() = -0.5 * height;
}

void ProbabilityGrid::update(const Isometry2f &robot_transform,
                             const std::vector<Vector2f> &observations) {
  const Vector2i robot_cell = world_to_cell(robot_transform.translation());

  for (const auto &obs : observations) {
    Vector2f obs_world = robot_transform * obs;
    Vector2i obs_cell = world_to_cell(obs_world);
    auto cells = get_ray_cells(robot_cell, obs_cell);

    for (int i = 0; i < cells.size() - 1; i++) {
      add(cells[i], -0.4);
    }

    add(cells.back(), 0.85);
  }
}

void ProbabilityGrid::add(const Vector2i &cell, float val) {
  if (in_bounds(cell)) {
    grid[cell.y() * width + cell.x()] += val;
  }
}

float ProbabilityGrid::at(int x, int y) const { return grid[y * width + x]; }

bool ProbabilityGrid::in_bounds(const Vector2i &cell) const {
  return cell.x() >= 0 && cell.x() < width && cell.y() >= 0 &&
         cell.y() < height;
}

Vector2i ProbabilityGrid::world_to_cell(const Vector2f world_pos) {
  return (grid_trans.inverse() * world_pos / resolution - origin).cast<int>();
}

std::vector<Vector2i> ProbabilityGrid::get_ray_cells(const Vector2i &cell0,
                                                     const Vector2i &cell1) {
  // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
  std::vector<Vector2i> cells;

  Vector2i diff = cell1 - cell0;

  int dx = std::abs(diff.x());
  int dy = -std::abs(diff.y());
  int sx = diff.x() > 0 ? 1 : -1;
  int sy = diff.y() > 0 ? 1 : -1;
  int err = dx + dy;

  int x = cell0.x();
  int y = cell0.y();

  while (1) {
    cells.emplace_back(x, y);
    int e2 = err * 2;
    if (e2 >= dy) {
      if (x == cell1.x())
        break;
      err += dy;
      x += sx;
    }
    if (e2 <= dx) {
      if (y == cell1.y())
        break;
      err += dx;
      y += sy;
    }
  }

  return cells;
}

void ProbabilityGrid::save_as_pgm(const std::string &file_name) {
  std::ofstream out(file_name, std::ios::binary);
  out << "P5\n" << width << " " << height << "\n255\n";

  for (int j = height - 1; j >= 0; --j) {
    for (int i = 0; i < width; ++i) {
      float logodds = std::clamp(at(i, j), -2.f, 3.5f);
      float prob = 1.0f - 1.0f / (1.0f + std::exp(logodds));
      uint8_t val;
      if (logodds == 0.0f)
        val = 128;
      else
        val = static_cast<uint8_t>(255 * (1.0f - prob));
      out.put(val);
    }
  }
}
