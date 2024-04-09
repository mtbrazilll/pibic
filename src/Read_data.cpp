#include "Read_data.hpp"



void read_points(const std::string& filepath, std::vector<Point> &points, K::FT &max_x, K::FT &max_y, K::FT &min_x, K::FT &min_y) {

    std::ifstream filePontos(filepath);

    if (!filePontos.is_open()) {
        
        return;
    }

    std::string line;
    double x, y;
    max_x = std::numeric_limits<K::FT>::lowest();
    max_y = std::numeric_limits<K::FT>::lowest();
    min_x = std::numeric_limits<K::FT>::max();
    min_y = std::numeric_limits<K::FT>::max();
    int id = 0;
    while (getline(filePontos, line)) {
        std::istringstream iss(line);
        if (!(iss >> x >> y)) {
            continue;
        }

        K::Point_2 point(x, y);
        points.push_back(point);
        tree.insert(point);
        pointToIndex[point] = id;
        
        max_x = std::max(max_x, point.x());
        max_y = std::max(max_y, point.y());
        min_x = std::min(min_x, point.x());
        min_y = std::min(min_y, point.y());
        id++;
    }
    
}
