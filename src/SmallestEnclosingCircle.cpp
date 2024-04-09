// SmallestEnclosingCircle.cpp

#include "SmallestEnclosingCircle.hpp"

Circle makeSmallestEnclosingCircle(const std::vector<Point>& points) {
    Min_circle mc(points.begin(), points.end(), true);
    Traits::Circle c = mc.circle();

    Circle result;
    
    result.c = c.center();
    result.r = std::sqrt(c.squared_radius());
    result.pos = c.center();
    return result;
}
