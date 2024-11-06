#include <cmath>
#include <unordered_map>
#include <vector>
#include <random>
#include "Componentes.hpp"
#include "SmallestEnclosingCircle.hpp"
#include <CGAL/Cartesian.h>

extern ComponentManager manager;
extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<> distr;
extern double raio;

class hexa {
    std::vector<Ponto> &P;
    double num1;
    double num2;

    // Constantes para o grid hexagonal
    double hexRadius = raio;  // O raio do círculo inscrito no hexágono

    struct BoundingBox {
        double minX, maxX, minY, maxY;
        std::vector<Ponto> pontos;
        BoundingBox() {
            minX = minY = DBL_MAX;
            maxX = maxY = DBL_MIN;
        }
        explicit BoundingBox(const Point &p, const Ponto &p1) : minX(p.x()), maxX(p.x()), minY(p.y()), maxY(p.y()) {
            pontos.push_back(p1);
        }
        void update(const Point &p, const Ponto &p1) {
            pontos.push_back(p1);
            minX = std::min(minX, p.x());
            minY = std::min(minY, p.y());
            maxX = std::max(maxX, p.x());
            maxY = std::max(maxY, p.y());
        }
    };

    typedef std::pair<int, int> HexCoord;  // Coordenadas axiais
    typedef std::pair<BoundingBox, bool> diskInfo;
    typedef std::unordered_map<HexCoord, diskInfo, boost::hash<HexCoord>> HashMap;

    // Função para converter coordenadas cartesianas para coordenadas axiais do hexágono
    inline HexCoord hexRound(double x, double y) {
        double q = (sqrt(3)/3 * x - 1.0/3 * y) / hexRadius;
        double r = (2.0/3 * y) / hexRadius;

        double s = -q - r;

        int rq = round(q);
        int rr = round(r);
        int rs = round(s);

        double q_diff = fabs(rq - q);
        double r_diff = fabs(rr - r);
        double s_diff = fabs(rs - s);

        if (q_diff > r_diff && q_diff > s_diff) {
            rq = -rr - rs;
        } else if (r_diff > s_diff) {
            rr = -rq - rs;
        } else {
            rs = -rq - rr;
        }

        return std::make_pair(rq, rr);
    }

    // Função para converter coordenadas axiais para cartesianas
    inline Point hexToPixel(int q, int r) {
        double x = hexRadius * sqrt(3) * (q + r / 2.0);
        double y = hexRadius * 1.5 * r;
        return Point(x, y);
    }

    // Função para obter os vizinhos de um hexágono
    std::vector<HexCoord> getHexNeighbors(const HexCoord &hex) {
        std::vector<HexCoord> neighbors = {
            {hex.first + 1, hex.second},     // Leste
            {hex.first - 1, hex.second},     // Oeste
            {hex.first, hex.second + 1},     // Nordeste
            {hex.first, hex.second - 1},     // Sudoeste
            {hex.first + 1, hex.second - 1}, // Sudeste
            {hex.first - 1, hex.second + 1}  // Noroeste
        };
        return neighbors;
    }

    inline bool trytoMergeDisk(HashMap &H, HashMap::iterator &iterToSourceDisk, const HexCoord &neighborHex) {
        auto iterToTargetDisk = H.find(neighborHex);

        if (iterToTargetDisk == H.end())
            return false;

        if (iterToTargetDisk->second.second) {
            double minX = std::min(iterToSourceDisk->second.first.minX, iterToTargetDisk->second.first.minX);
            double minY = std::min(iterToSourceDisk->second.first.minY, iterToTargetDisk->second.first.minY);
            double maxX = std::max(iterToSourceDisk->second.first.maxX, iterToTargetDisk->second.first.maxX);
            double maxY = std::max(iterToSourceDisk->second.first.maxY, iterToTargetDisk->second.first.maxY);

            Point lowerLeft(minX, minY), upperRight(maxX, maxY);

            double maxDistanceSquared = (2 * raio) * (2 * raio);
            if (CGAL::squared_distance(lowerLeft, upperRight) <= maxDistanceSquared) {
                iterToSourceDisk->second.second = false;
                iterToTargetDisk->second.second = false;

                iterToTargetDisk->second.first.pontos.insert(
                    iterToTargetDisk->second.first.pontos.end(),
                    iterToSourceDisk->second.first.pontos.begin(),
                    iterToSourceDisk->second.first.pontos.end());

                // Calcula o centro do novo disco
                Point centro((lowerLeft.x() + upperRight.x()) / 2 - num1,
                             (lowerLeft.y() + upperRight.y()) / 2 - num2);

                Component aux(raio, iterToTargetDisk->second.first.pontos, centro);
                manager.addComponent(aux);

                return true;
            }
        }
        return false;
    }

public:
    hexa(std::vector<Ponto> &P) : P(P) {}

    double execute() {
        num1 = distr(gen);
        num2 = distr(gen);
        HashMap H;

        for (const Ponto &p : P) {
            // Translada os pontos
            Point trans(p.point.x() + num1, p.point.y() + num2);

            // Calcula as coordenadas axiais do hexágono
            HexCoord hex = hexRound(trans.x(), trans.y());

            // Calcula o centro do hexágono
            Point hexCenter = hexToPixel(hex.first, hex.second);

            // Verifica se o ponto está dentro do hexágono
            if (CGAL::squared_distance(trans, hexCenter) <= hexRadius * hexRadius) {
                auto it = H.find(hex);
                if (it != H.end()) {
                    it->second.first.update(trans, p);
                } else {
                    H[hex] = std::make_pair(BoundingBox(trans, p), true);
                }
            } else {
                // Verifica os hexágonos vizinhos
                std::vector<HexCoord> neighbors = getHexNeighbors(hex);
                bool added = false;
                for (const auto &neighborHex : neighbors) {
                    Point neighborCenter = hexToPixel(neighborHex.first, neighborHex.second);
                    if (CGAL::squared_distance(trans, neighborCenter) <= hexRadius * hexRadius) {
                        auto it = H.find(neighborHex);
                        if (it != H.end()) {
                            it->second.first.update(trans, p);
                        } else {
                            H[neighborHex] = std::make_pair(BoundingBox(trans, p), true);
                        }
                        added = true;
                        break;
                    }
                }
                if (!added) {
                    // Se o ponto não estiver dentro de nenhum hexágono, crie um novo no hex atual
                    H[hex] = std::make_pair(BoundingBox(trans, p), true);
                }
            }
        }

        for (auto iter = H.begin(); iter != H.end(); ++iter) {
            if (!iter->second.second) {
                continue;
            }

            // Tenta mesclar com os discos vizinhos
            std::vector<HexCoord> neighbors = getHexNeighbors(iter->first);
            bool merged = false;
            for (const auto &neighborHex : neighbors) {
                if (trytoMergeDisk(H, iter, neighborHex)) {
                    merged = true;
                    break;
                }
            } 

            if (!merged) {
                // Cria o componente para o hexágono atual
                Point centro = hexToPixel(iter->first.first, iter->first.second);
                centro = Point(centro.x() - num1, centro.y() - num2);
                Component aux(raio, iter->second.first.pontos, centro);
                manager.addComponent(aux);
            }
        }

        return 0;
    }
};
