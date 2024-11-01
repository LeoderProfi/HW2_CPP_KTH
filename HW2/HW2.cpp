#include <iostream>
#include <vector>
#include "mpl_writer.hpp"
#include "random_points.hpp"
#include "random_points.hpp"

class Point {
public:
    double x;
    double y;

    // Default constructor
    Point() : x(0), y(0) {}

    // Constructor with coordinates
    Point(double x_coordinate, double y_coordinate) : x(x_coordinate), y(y_coordinate) {}
};

class Rectangle {
public:
    Point bottomLeft;
    Point topRight;

    Rectangle(Point bl, Point ur) : bottomLeft(bl), topRight(ur) {}

    // Method to create four equally distant sub-rectangles
    std::vector<Rectangle> subRects() const {
        std::vector<Rectangle> sub_rectangles;

        // Find the midpoint of the rectangle
        double mid_x = (bottomLeft.x + topRight.x) / 2;
        double mid_y = (bottomLeft.y + topRight.y) / 2;

        // Define the four sub-rectangles
        Rectangle bottom_left_rect(bottomLeft, Point(mid_x, mid_y));
        Rectangle bottom_right_rect(Point(mid_x, bottomLeft.y), Point(topRight.x, mid_y));
        Rectangle top_left_rect(Point(bottomLeft.x, mid_y), Point(mid_x, topRight.y));
        Rectangle top_right_rect(Point(mid_x, mid_y), topRight);

        // Add them to the vector
        sub_rectangles.push_back(bottom_left_rect);
        sub_rectangles.push_back(bottom_right_rect);
        sub_rectangles.push_back(top_left_rect);
        sub_rectangles.push_back(top_right_rect);

        return sub_rectangles;
    }
};

class QuadTree {
public:
    std::vector<Point> Points;
    int BucketTreeCap;
    std::vector<std::pair<Rectangle, std::vector<Point>>> Leafs;

    // Method to find points within a rectangle
    std::vector<Point> P_in_rect(const std::vector<Point>& Points, const Rectangle& rect) {
        std::vector<Point> Points_in_rectangle;

        for (const auto& point : Points) {
            if (point.x > rect.bottomLeft.x && point.x < rect.topRight.x &&
                point.y > rect.bottomLeft.y && point.y < rect.topRight.y) {
                Points_in_rectangle.push_back(point);
            }
        }
        return Points_in_rectangle;
    }

    // Method to check if a split is needed based on bucket capacity
    bool split_needed(const std::vector<Point>& Points_in_rectangle, int BucketTreeCap) {
        return Points_in_rectangle.size() > BucketTreeCap;
    }

    // Recursive method to build the quadtree
    void quadtreestuff(std::vector<std::pair<Rectangle, std::vector<Point>>>& Leafs,
        const std::vector<Point>& Points, int BucketTreeCap, const Rectangle& rect) {
        std::vector<Point> points_in_rect = P_in_rect(Points, rect);

        if (!split_needed(points_in_rect, BucketTreeCap)) {
            Leafs.emplace_back(rect, points_in_rect);
            return;
        }
        else {
            for (const auto& subrectangle : rect.subRects()) {
                quadtreestuff(Leafs, points_in_rect, BucketTreeCap, subrectangle);
            }
        }
    }


};


int main() {
    // Generate random points using the provided RandomPointGenerator
    sf::RandomPointGenerator<Point> generator{ 2565 }; // Use 2565 as the seed (reproducible)
    generator.addNormalPoints(50);
    auto points = generator.takePoints();

    
    // Define the main boundary for the QuadTree (adjust as needed)
    Rectangle boundary(Point(-5, -5), Point(5, 5));
    QuadTree quadtree;
    quadtree.BucketTreeCap = 5;  // Set the bucket capacity

    // Build the quadtree
    quadtree.quadtreestuff(quadtree.Leafs, points, quadtree.BucketTreeCap, boundary);

    // Write to a matplotlib script using MplWriter
    sf::MplWriter<Point, Rectangle> writer("plot.py", 10.0); // Marker size of 20.0 for points

    for (const auto& leaf : quadtree.Leafs) {
        writer << leaf.first;  // Write rectangle
        writer << leaf.second; // Write points within the rectangle
    }

    // Write the points to the plot
    //writer << points;

    std::cout << "Plot script generated: plot.py" << std::endl;
    return 0;
}