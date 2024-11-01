#include <vector>

#include "quadtree.hpp"
#include "utils/mpl_writer.hpp"
#include "utils/point_reader.hpp"
#include "utils/random_points.hpp"
#include "utils/timer.hpp"

void write_dataset(std::string inputFile, std::string outputFile = "example.py",
                   size_t capacity = 64) {
  std::vector<Point> points = sf::readCsvPoints<Point>(inputFile);

  sf::MplWriter<Point, Rectangle> mpl(outputFile);
  QuadTree qt = QuadTree(points, capacity);

  qt.traverse(
      [&mpl](const std::vector<Point>& points, const Rectangle& boundary) {
        mpl << boundary;
        mpl << points;
      });
}

std::vector<Point> naive_lookup(const std::vector<Point>& points,
                                const Rectangle& boundary) {
  std::vector<Point> found;
  for (auto& p : points) {
    if (boundary.contains(p)) found.push_back(p);
  }

  return found;
}

int main(int argc, char* argv[]) {
  sf::Timer timer;

  if (argc > 1) {
    timer.start("Sweden query");
    write_dataset(argv[1], "output.py");
    timer.stop();
  }

  size_t testCapacities[] = {32, 64, 128, 512, 2048};
  double testPointCounts[] = {1e3, 1e4, 1e6, 1e7};
  Point testRegions[] = {{0.0, 0.0}, {1.0, 1.0}, {0.5, 0.5}};

  sf::RandomPointGenerator<Point> generator{2565};

  for (auto pointCount : testPointCounts) {
    generator.addNormalPoints(size_t(pointCount));
    auto points = generator.takePoints();

    for (auto capacity : testCapacities) {
      std::cout << "Running test with parameters: " << std::endl
                << "\tCapacity: " << capacity << std::endl
                << "\tPoint count: " << pointCount << std::endl;

      timer.start("Building quadtree");
      QuadTree qt = QuadTree(points, capacity);
      timer.stop();

      for (auto region : testRegions) {
        std::cout << "Querying region: " << region.x << ", " << region.y
                  << std::endl;

        auto queryRegion =
            Rectangle(region, Point(region.x + 0.02, region.y + 0.02));

        timer.start("Querying quadtree");
        qt.query(queryRegion);
        timer.stop();

        timer.start("Naive lookup");
        naive_lookup(points, queryRegion);
        timer.stop();
      }
    }
  }
}
