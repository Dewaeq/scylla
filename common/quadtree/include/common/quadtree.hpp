#ifndef QUADTREE_H
#define QUADTREE_H

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/src/Core/Matrix.h>
#include <memory>
#include <vector>

struct BoundingBox {
  Eigen::Vector2f center;
  Eigen::Vector2f half_dimension;

  bool contains(const Eigen::Vector2f &point);
  bool intersects(const BoundingBox &other);
};

class Quadtree {
public:
  Quadtree(BoundingBox boundary, int capacity);
  bool insert(const Eigen::Vector2f &point);
  std::vector<Eigen::Vector2f> query(const BoundingBox &range);

private:
  void subdivide();

  BoundingBox boundary;
  int capacity;
  std::vector<Eigen::Vector2f> points;
  bool is_divided;

  std::unique_ptr<Quadtree> nw_tree;
  std::unique_ptr<Quadtree> ne_tree;
  std::unique_ptr<Quadtree> se_tree;
  std::unique_ptr<Quadtree> sw_tree;
};

#endif // !QUADTREE_H
