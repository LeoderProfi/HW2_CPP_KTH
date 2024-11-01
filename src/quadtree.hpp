#pragma once

#include "geometry.hpp"
#include <memory>
#include <functional>
#include <vector>

class QuadTree {
public:
  static const int DEFAULT_CAPACITY = 10;
  QuadTree(std::vector<Point> &points, int capacity = DEFAULT_CAPACITY)
      : capacity(capacity) {

    if (points.empty())
      return;

    auto bottomLeft = Point(points[0]);
    auto topRight = Point(points[0]);
    for (Point &p : points) {
      if (p.x < bottomLeft.x)
        bottomLeft.x = p.x;
      if (p.y < bottomLeft.y)
        bottomLeft.y = p.y;
      if (p.x > topRight.x)
        topRight.x = p.x;
      if (p.y > topRight.y)
        topRight.y = p.y;
    }

    boundary = Rectangle(bottomLeft, topRight);
    for (Point &p : points)
      insert(p);
  }

  QuadTree(Rectangle boundary, int capacity = DEFAULT_CAPACITY)
      : boundary(boundary), capacity(capacity) {}

  QuadTree(const QuadTree &) = delete;
  QuadTree &operator=(const QuadTree &) = delete;

  size_t size() { return points.size(); }

  bool insert(const Point &point) {
    if (!boundary.contains(point))
      return false;

    if (!has_split()) {
      points.emplace_back(point);
      if (points.size() > capacity)
        split();

      return true;
    } else {
      return (northWest->insert(point) || northEast->insert(point) ||
              southWest->insert(point) || southEast->insert(point));
    }
  }
  
  /**
   * @brief Traverses the quadtree and applies a function to each node.
   *
   * This function recursively traverses the quadtree. It will iterate over each leaf node
   * and apply the function `iterate` to the leaf node. The nodes are iterated in a depth-first
   * manner.
   * 
   *
   * @param iterate A callback function that takes the vector of Points and boundary of the iterated node.
   */
  void traverse(const std::function<void(const std::vector<Point>&, const Rectangle&)>& iterate) {
    if (has_split()) {
      northWest->traverse(iterate);
      northEast->traverse(iterate);
      southWest->traverse(iterate);
      southEast->traverse(iterate);
    } else {
      iterate(points, boundary);
    }
  }

  /**
   * @brief Traverses the quadtree and applies a function to points within a given range.
   *
   * This function recursively traverses the quadtree, checking if the specified range overlaps
   * with the boundary of the current node. The traversal is similar to the `traverse` function,
   * but it will only apply the function `iterate` to the points within the range.
   *
   * @param range The rectangular range to query within the quadtree.
   * @param iterate A function to apply to the points found within the range and the boundary
   *                of the current node.
   */
  void traverseQuery(Rectangle range, const std::function<void(const std::vector<Point>&, const Rectangle&)>& iterate) {
    if (!boundary.overlaps(range))
      return;

    if (has_split()) {
      northWest->traverseQuery(range, iterate);
      northEast->traverseQuery(range, iterate);
      southWest->traverseQuery(range, iterate);
      southEast->traverseQuery(range, iterate);
    } else {
      std::vector<Point> found;
      for (Point &p : points) {
        if (range.contains(p))
          found.emplace_back(p);
      }
      iterate(found, boundary);
    }
  }

  std::vector<Point> query(Rectangle range) {
    std::vector<Point> found;

    traverseQuery(range, [&found](const std::vector<Point>& points, const Rectangle& boundary) {
      found.insert(found.end(), points.begin(), points.end());
    });

    return found;
  }

private:
  void split() {
    const double top = boundary.topRight.y;
    const double bottom = boundary.bottomLeft.y;
    const double left = boundary.bottomLeft.x;
    const double right = boundary.topRight.x;

    Point center = boundary.center();

    Rectangle boundaryNW =
        Rectangle(Point(left, center.y), Point(center.x, top));
    Rectangle boundaryNE = Rectangle(center, boundary.topRight);
    Rectangle boundarySW = Rectangle(boundary.bottomLeft, center);
    Rectangle boundarySE =
        Rectangle(Point(center.x, bottom), Point(right, center.y));

    northWest = std::make_unique<QuadTree>(boundaryNW, capacity);
    northEast = std::make_unique<QuadTree>(boundaryNE, capacity);
    southWest = std::make_unique<QuadTree>(boundarySW, capacity);
    southEast = std::make_unique<QuadTree>(boundarySE, capacity);

    for (auto &point : points) {
      insert(point);
    }

    points.clear();
  }

  bool has_split() { return northWest != nullptr; }

  int capacity;
  Rectangle boundary;
  std::vector<Point> points;
  std::unique_ptr<QuadTree> northWest = nullptr;
  std::unique_ptr<QuadTree> northEast = nullptr;
  std::unique_ptr<QuadTree> southWest = nullptr;
  std::unique_ptr<QuadTree> southEast = nullptr;
};
