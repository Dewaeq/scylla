#include <eigen3/Eigen/Dense>
#include <memory>

#include "common/quadtree.hpp"

using namespace Eigen;

bool BoundingBox::contains(const Vector2f &point) {
  return point.x() >= center.x() - half_dimension.x() &&
         point.x() <= center.x() + half_dimension.x() &&
         point.y() >= center.y() - half_dimension.y() &&
         point.y() <= center.y() + half_dimension.y();
}

bool BoundingBox::intersects(const BoundingBox &other) {
  return (std::abs(center.x() - other.center.x()) <
          half_dimension.x() + other.half_dimension.x()) &&
         (std::abs(center.y() - other.center.y()) <
          half_dimension.y() + other.half_dimension.y());
}

Quadtree::Quadtree(BoundingBox boundary, int capacity)
    : boundary(boundary), capacity(capacity), is_divided(false) {}

bool Quadtree::insert(const Vector2f &point) {
  if (!boundary.contains(point))
    return false;

  if (points.size() < capacity) {
    points.push_back(point);
    return true;
  }

  if (!is_divided)
    subdivide();

  if (nw_tree->insert(point))
    return true;
  if (ne_tree->insert(point))
    return true;
  if (se_tree->insert(point))
    return true;
  if (sw_tree->insert(point))
    return true;

  return false;
}

void Quadtree::subdivide() {
  Vector2f child_half_dim = boundary.half_dimension / 2.;

  BoundingBox nw_bound(Vector2f(boundary.center.x() - child_half_dim.x(),
                                boundary.center.y() - child_half_dim.y()),
                       child_half_dim);
  BoundingBox ne_bound(Vector2f(boundary.center.x() + child_half_dim.x(),
                                boundary.center.y() - child_half_dim.y()),
                       child_half_dim);
  BoundingBox sw_bound(Vector2f(boundary.center.x() - child_half_dim.x(),
                                boundary.center.y() + child_half_dim.y()),
                       child_half_dim);
  BoundingBox se_bound(Vector2f(boundary.center.x() + child_half_dim.x(),
                                boundary.center.y() + child_half_dim.y()),
                       child_half_dim);

  nw_tree = std::make_unique<Quadtree>(nw_bound, capacity);
  ne_tree = std::make_unique<Quadtree>(ne_bound, capacity);
  sw_tree = std::make_unique<Quadtree>(sw_bound, capacity);
  se_tree = std::make_unique<Quadtree>(se_bound, capacity);

  is_divided = true;

  for (const auto &point : points) {
    insert(point);
  }

  points.clear();
}
