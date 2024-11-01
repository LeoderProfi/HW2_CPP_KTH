#include "quadtree.hpp"
#include "utils/mpl_writer.hpp"
#include "utils/point_reader.hpp"
#include "utils/random_points.hpp"
#include "utils/timer.hpp"
#include <vector>

void test_sweden() {
  std::vector<Point> points = sf::readCsvPoints<Point>(
      "/Users/aast/projs/cpp-scicomp/assignment2/test_data/swe.csv");

  sf::MplWriter<Point, Rectangle> mpl("example.py");
  QuadTree qt = QuadTree(points, 64);

  qt.traverse(
      [&mpl](const std::vector<Point> &points, const Rectangle &boundary) {
        mpl << boundary;
        if (points.size() < 20)
          return;
        mpl << points;
      });
}

int main() {
  sf::RandomPointGenerator<Point> generator{2565};
  generator.addNormalPoints(10e4);

  auto points = generator.takePoints();

  sf::Timer timer;

  timer.start("Building quadtree");
  QuadTree qt = QuadTree(points, 2048);
  timer.stop();
  sf::MplWriter<Point, Rectangle> mpl("example_query.py");

  timer.start("traversing full quadtree");
  qt.traverse([&mpl](const std::vector<Point> &points,
                     const Rectangle &boundary) { mpl << boundary; });
  timer.stop();

  auto queryRange = Rectangle(Point(1., 1.), Point(1.05, 1.05));
  mpl << queryRange;
  timer.start("Querying quadtree");
  qt.traverseQuery(queryRange, [&mpl](const std::vector<Point> &points,
                                      const Rectangle &boundary) {
    mpl << boundary;
    mpl << points;
  });
  timer.stop();
}
