#ifndef DISTANCE_CACHE_H
#define DISTANCE_CACHE_H

#include <vector>
#include "Struct.h"

struct DistanceCache {
    std::vector<std::vector<double>> distances;
    
    void initialize(const std::vector<Ponto>& points) {
        size_t n = points.size();
        distances.resize(n, std::vector<double>(n));
        
        // Pré-calcula todas as distâncias quadradas
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = i; j < n; ++j) {
                double dist = CGAL::squared_distance(points[i].point, points[j].point);
                distances[i][j] = distances[j][i] = dist;
            }
        }
    }
    
    double getSquaredDistance(size_t i, size_t j) const {
        return distances[i][j];
    }
};

extern DistanceCache cache;

#endif