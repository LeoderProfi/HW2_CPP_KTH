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

    std::vector<Rectangle> subRects() const {
        std::vector<Rectangle> sub_rectangles;

        double mid_x = (bottomLeft.x + topRight.x) / 2;
        double mid_y = (bottomLeft.y + topRight.y) / 2;

        Rectangle bottom_left_rect(bottomLeft, Point(mid_x, mid_y));
        Rectangle bottom_right_rect(Point(mid_x, bottomLeft.y), Point(topRight.x, mid_y));
        Rectangle top_left_rect(Point(bottomLeft.x, mid_y), Point(mid_x, topRight.y));
        Rectangle top_right_rect(Point(mid_x, mid_y), topRight);

        sub_rectangles.push_back(bottom_left_rect);
        sub_rectangles.push_back(bottom_right_rect);
        sub_rectangles.push_back(top_left_rect);
        sub_rectangles.push_back(top_right_rect);

        return sub_rectangles;
    }

    // Method to check if a point is within this rectangle
    bool contains(const Point& point) const {
        return (point.x >= bottomLeft.x && point.x <= topRight.x &&
            point.y >= bottomLeft.y && point.y <= topRight.y);
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

    bool rectangleoverlap(const Rectangle& rect1, const Rectangle& rect2) {
        if (rect1.topRight.x <= rect2.bottomLeft.x || rect2.topRight.x <= rect1.bottomLeft.x) {
            return false;
        }
        if (rect1.topRight.y <= rect2.bottomLeft.y || rect2.topRight.y <= rect1.bottomLeft.y) {
            return false;
        }
        return true;
    }


    void query(std::vector<std::pair<Rectangle, std::vector<Point>>>& Leafs, const Rectangle& rect, std::vector<Point>& result) {
        for (const auto& leaf : Leafs) {
            const Rectangle& leafRect = leaf.first;            // The rectangle in the current leaf
            const std::vector<Point>& pointsInLeaf = leaf.second; // Points in the current leaf

            // Check if the leaf's rectangle overlaps with the query rectangle
            if (rectangleoverlap(leafRect, rect)) {
                // If they overlap, check each point in this leaf
                for (const auto& point : pointsInLeaf) {
                    // Add the point to the result if it lies within the query rectangle
                    if (rect.contains(point)) {
                        result.push_back(point);
                    }
                }
            }
        }
    }

};




int main() {
    sf::RandomPointGenerator<Point> generator{ 2565 }; 
    generator.addNormalPoints(50);
    auto points = generator.takePoints();

    
    // Define the main boundary for the QuadTree (adjust as needed)
    Rectangle boundary(Point(-5, -5), Point(5, 5));
    Rectangle Query_rect(Point(-0.5, -0.5), Point(0.5, 0.5));
    QuadTree quadtree;
    quadtree.BucketTreeCap = 5;  // Set the bucket capacity

    // Build the quadtree
    quadtree.quadtreestuff(quadtree.Leafs, points, quadtree.BucketTreeCap, boundary);

    std::vector<Point> Result_query;

    quadtree.query(quadtree.Leafs, Query_rect, Result_query);

    //// Write to a matplotlib script using MplWriter
    //sf::MplWriter<Point, Rectangle> writer("plot.py", 10.0); // Marker size of 20.0 for points

    //for (const auto& leaf : quadtree.Leafs) {
    //    writer << leaf.first;  // Write rectangle
    //    writer << leaf.second; // Write points within the rectangle
    //}

    // Write to a matplotlib script using MplWriter
    sf::MplWriter<Point, Rectangle> writer("plot.py", 10.0); // Marker size of 20.0 for points

    for (const auto& leaf : quadtree.Leafs) {
        writer << Query_rect;  // Write rectangle
        writer << Result_query; // Write points within the rectangle
    }


    std::cout << "Plot script generated: plot.py" << std::endl;
    return 0;
}