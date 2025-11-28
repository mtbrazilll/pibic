//
// Created by Ghosh, Anirban on 11/11/21.
//

#ifndef FASTCOVERPP_H
#define FASTCOVERPP_H

#include <chrono>
#include <list>
#include <vector>
#include <unordered_set>
#include <random>
#include "Componentes.hpp"
#include "Struct.h"
extern ComponentManager manager;


extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<> distr;
extern double raio;


class FASTCOVER_PP {


    const std::vector<Ponto> &P;
    std::vector<Point> centers_points;


    double num1;
    double num2;

    const double sqrt2 = std::sqrt(2);
    double cellSize = sqrt2 * raio;
    double additiveFactor = cellSize / 2;
    double sqrt2TimesOnePointFiveMinusOne; // Será inicializado no construtor
    double sqrt2TimesZeroPointFivePlusOne; // Será inicializado no construtor

    struct BoundingBox {
        double minX, maxX, minY, maxY;
        std::vector<int> pontos_indices;
        BoundingBox() {
            minX = minY = DBL_MAX;
            maxX = maxY = DBL_MIN;
        }
        explicit BoundingBox(const Point &p, int p1_idx) : minX(p.x()), maxX(p.x()), minY(p.y()), maxY(p.y()) {
            pontos_indices.push_back(p1_idx);
        }
        void update(const Point &p, int p1_idx) {
            pontos_indices.push_back(p1_idx);
            minX = std::min(minX, p.x());
            minY = std::min(minY, p.y());
            maxX = std::max(maxX, p.x());
            maxY = std::max(maxY, p.y());
        }
    };

    typedef std::pair<int, int> intPair;
    typedef std::pair<BoundingBox, bool> diskInfo;
    typedef std::unordered_map<intPair, diskInfo, boost::hash<intPair>> HashMap;

    inline bool trytoMergeDisk(HashMap &H, HashMap::iterator &iterToSourceDisk, int vPrime, int hPrime, int &solution) {
        auto iterToTargetDisk = H.find(std::make_pair(vPrime, hPrime));

        if (iterToTargetDisk == H.end())
            return false;

        if (iterToTargetDisk->second.second) {

            double minX = std::min((*iterToSourceDisk).second.first.minX, iterToTargetDisk->second.first.minX);
            double minY = std::min((*iterToSourceDisk).second.first.minY, iterToTargetDisk->second.first.minY);
            double maxX = std::max((*iterToSourceDisk).second.first.maxX, iterToTargetDisk->second.first.maxX);
            double maxY = std::max((*iterToSourceDisk).second.first.maxY, iterToTargetDisk->second.first.maxY);

            Point lowerLeft(minX, minY), upperRight(maxX, maxY);

            double maxDistanceSquared = (2 * raio) * (2 * raio);
            if (CGAL::squared_distance(lowerLeft, upperRight) <= maxDistanceSquared) {
                (*iterToSourceDisk).second.second = false;
                iterToTargetDisk->second.second = false;

                iterToTargetDisk->second.first.pontos_indices.insert(iterToTargetDisk->second.first.pontos_indices.end(), iterToSourceDisk->second.first.pontos_indices.begin(), iterToSourceDisk->second.first.pontos_indices.end());
                Point centro = CGAL::midpoint(lowerLeft, upperRight);
                Component aux(raio, iterToTargetDisk->second.first.pontos_indices, Point(centro.x() - num1, centro.y() - num2));
                manager.addComponent(aux);
                ++solution;
                return true;
            }
        }
        return false;
    }

        inline bool trytoMergeDisk_dr(HashMap &H, HashMap::iterator &iterToSourceDisk, int vPrime, int hPrime, int &solution) {
        auto iterToTargetDisk = H.find(std::make_pair(vPrime, hPrime));

        if (iterToTargetDisk == H.end())
            return false;

        if (iterToTargetDisk->second.second) {

            double minX = std::min((*iterToSourceDisk).second.first.minX, iterToTargetDisk->second.first.minX);
            double minY = std::min((*iterToSourceDisk).second.first.minY, iterToTargetDisk->second.first.minY);
            double maxX = std::max((*iterToSourceDisk).second.first.maxX, iterToTargetDisk->second.first.maxX);
            double maxY = std::max((*iterToSourceDisk).second.first.maxY, iterToTargetDisk->second.first.maxY);

            Point lowerLeft(minX, minY), upperRight(maxX, maxY);

            double maxDistanceSquared = (2 * raio) * (2 * raio);
            if (CGAL::squared_distance(lowerLeft, upperRight) <= maxDistanceSquared) {
                (*iterToSourceDisk).second.second = false;
                iterToTargetDisk->second.second = false;

                iterToTargetDisk->second.first.pontos_indices.insert(iterToTargetDisk->second.first.pontos_indices.end(), iterToSourceDisk->second.first.pontos_indices.begin(), iterToSourceDisk->second.first.pontos_indices.end());
                Point centro = CGAL::midpoint(lowerLeft, upperRight);
                centers_points.push_back(centro);
                ++solution;
                return true;
            }
        }
        return false;
    }

public:
    FASTCOVER_PP(const std::vector<Ponto> &P) : P(P) {
        // Atualiza as constantes baseadas no raio
        sqrt2TimesOnePointFiveMinusOne = (sqrt2 * 1.5 * raio) - raio;
        sqrt2TimesZeroPointFivePlusOne = (sqrt2 * 0.5 * raio) + raio;
        
    }

    int execute() {

        num1 = distr(gen);
        num2 = distr(gen);
        HashMap H;
        int solution = 0;
        for (const Ponto &p : P) {

            Point trans(p.point.x() + num1, p.point.y() + num2);

            int v = floor(trans.x() / cellSize), h = floor(trans.y() / cellSize);

            double verticalTimesCellSize = v * cellSize, horizontalTimesCellSize = h * cellSize;

            auto it = H.find(std::make_pair(v, h));
            if (it != H.end()) {
                it->second.first.update(trans, p.indice);
                continue;
            }

            if ((trans.x() >= verticalTimesCellSize + sqrt2TimesOnePointFiveMinusOne)) {
                it = H.find(std::make_pair(v + 1, h));
                if (it != H.end() && (CGAL::squared_distance(trans, Point(cellSize * (v + 1) + additiveFactor, horizontalTimesCellSize + additiveFactor)) <= raio * raio)) {
                    it->second.first.update(trans, p.indice);
                    continue;
                }
            }

            if ((trans.x() <= verticalTimesCellSize - sqrt2TimesZeroPointFivePlusOne)) {
                it = H.find(std::make_pair(v - 1, h));
                if (it != H.end() && (CGAL::squared_distance(trans, Point(cellSize * (v - 1) + additiveFactor, horizontalTimesCellSize + additiveFactor)) <= raio * raio)) {
                    it->second.first.update(trans, p.indice);
                    continue;
                }
            }

            if ((trans.y() <= horizontalTimesCellSize + sqrt2TimesOnePointFiveMinusOne)) {
                it = H.find(std::make_pair(v, h - 1));
                if (it != H.end() && (CGAL::squared_distance(trans, Point(verticalTimesCellSize + additiveFactor, cellSize * (h - 1) + additiveFactor)) <= raio * raio)) {
                    it->second.first.update(trans, p.indice);
                    continue;
                }
            }

            if ((trans.y() >= horizontalTimesCellSize - sqrt2TimesZeroPointFivePlusOne)) {
                it = H.find(std::make_pair(v, h + 1));
                if (it != H.end() && (CGAL::squared_distance(trans, Point(verticalTimesCellSize + additiveFactor, cellSize * (h + 1) + additiveFactor)) <= raio * raio)) {
                    it->second.first.update(trans, p.indice);
                    continue;
                }
            }
            H[std::make_pair(v, h)] = std::make_pair(BoundingBox(trans, p.indice), true);
        }

        for (auto iter = H.begin(); iter != H.end(); ++iter) {
            int v = (*iter).first.first, h = (*iter).first.second;
            if (!(*iter).second.second) {
                continue;
            }

            // Tenta mesclar com o disco S
            if (trytoMergeDisk(H, iter, v, h - 1,solution))
                continue;

            // Tenta mesclar com o disco N
            if (trytoMergeDisk(H, iter, v, h + 1,solution))
                continue;

            // Tenta mesclar com o disco E
            if (trytoMergeDisk(H, iter, v + 1, h,solution))
                continue;

            // Tenta mesclar com o disco W
            if (trytoMergeDisk(H, iter, v - 1, h,solution))
                continue;

            // Tenta mesclar com o disco SW
            if (trytoMergeDisk(H, iter, v - 1, h - 1,solution))
                continue;

            // Tenta mesclar com o disco SE
            if (trytoMergeDisk(H, iter, v + 1, h - 1,solution))
                continue;

            // Tenta mesclar com o disco NE
            if (trytoMergeDisk(H, iter, v + 1, h + 1,solution))
                continue;

            // Tenta mesclar com o disco NW
            if (trytoMergeDisk(H, iter, v - 1, h + 1,solution))
                continue;
        }

        for (auto aPair : H) {
            if (aPair.second.second) {
                Point centro(aPair.first.first * cellSize + additiveFactor, aPair.first.second * cellSize + additiveFactor);

                Component aux(raio, aPair.second.first.pontos_indices, Point(centro.x() - num1, centro.y() - num2));
                manager.addComponent(aux);
                solution++;
                
            }
        }

        return solution;
    }

        std::vector<Point> execute_dr() {

        num1 = distr(gen);
        num2 = distr(gen);
        HashMap H;
        int solution = 0;
        for (const Ponto &p : P) {

            Point trans(p.point.x() + num1, p.point.y() + num2);

            int v = floor(trans.x() / cellSize), h = floor(trans.y() / cellSize);

            double verticalTimesCellSize = v * cellSize, horizontalTimesCellSize = h * cellSize;

            auto it = H.find(std::make_pair(v, h));
            if (it != H.end()) {
                it->second.first.update(trans, p.indice);
                continue;
            }

            if ((trans.x() >= verticalTimesCellSize + sqrt2TimesOnePointFiveMinusOne)) {
                it = H.find(std::make_pair(v + 1, h));
                if (it != H.end() && (CGAL::squared_distance(trans, Point(cellSize * (v + 1) + additiveFactor, horizontalTimesCellSize + additiveFactor)) <= raio * raio)) {
                    it->second.first.update(trans, p.indice);
                    continue;
                }
            }

            if ((trans.x() <= verticalTimesCellSize - sqrt2TimesZeroPointFivePlusOne)) {
                it = H.find(std::make_pair(v - 1, h));
                if (it != H.end() && (CGAL::squared_distance(trans, Point(cellSize * (v - 1) + additiveFactor, horizontalTimesCellSize + additiveFactor)) <= raio * raio)) {
                    it->second.first.update(trans, p.indice);
                    continue;
                }
            }

            if ((trans.y() <= horizontalTimesCellSize + sqrt2TimesOnePointFiveMinusOne)) {
                it = H.find(std::make_pair(v, h - 1));
                if (it != H.end() && (CGAL::squared_distance(trans, Point(verticalTimesCellSize + additiveFactor, cellSize * (h - 1) + additiveFactor)) <= raio * raio)) {
                    it->second.first.update(trans, p.indice);
                    continue;
                }
            }

            if ((trans.y() >= horizontalTimesCellSize - sqrt2TimesZeroPointFivePlusOne)) {
                it = H.find(std::make_pair(v, h + 1));
                if (it != H.end() && (CGAL::squared_distance(trans, Point(verticalTimesCellSize + additiveFactor, cellSize * (h + 1) + additiveFactor)) <= raio * raio)) {
                    it->second.first.update(trans, p.indice);
                    continue;
                }
            }
            H[std::make_pair(v, h)] = std::make_pair(BoundingBox(trans, p.indice), true);
        }

        for (auto iter = H.begin(); iter != H.end(); ++iter) {
            int v = (*iter).first.first, h = (*iter).first.second;
            if (!(*iter).second.second) {
                continue;
            }

            // Tenta mesclar com o disco S
            if (trytoMergeDisk_dr(H, iter, v, h - 1,solution))
                continue;

            // Tenta mesclar com o disco N
            if (trytoMergeDisk_dr(H, iter, v, h + 1,solution))
                continue;

            // Tenta mesclar com o disco E
            if (trytoMergeDisk_dr(H, iter, v + 1, h,solution))
                continue;

            // Tenta mesclar com o disco W
            if (trytoMergeDisk_dr(H, iter, v - 1, h,solution))
                continue;

            // Tenta mesclar com o disco SW
            if (trytoMergeDisk_dr(H, iter, v - 1, h - 1,solution))
                continue;

            // Tenta mesclar com o disco SE
            if (trytoMergeDisk_dr(H, iter, v + 1, h - 1,solution))
                continue;

            // Tenta mesclar com o disco NE
            if (trytoMergeDisk_dr(H, iter, v + 1, h + 1,solution))
                continue;

            // Tenta mesclar com o disco NW
            if (trytoMergeDisk_dr(H, iter, v - 1, h + 1,solution))
                continue;
        }
        
        for (auto aPair : H) {
            if (aPair.second.second) {
                Point centro(aPair.first.first * cellSize + additiveFactor, aPair.first.second * cellSize + additiveFactor);

                centers_points.push_back(centro);
            }
        }

        return centers_points;
    }


};

#endif // FASTCOVERPP_H
