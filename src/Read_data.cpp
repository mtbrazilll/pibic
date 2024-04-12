#include "Read_data.hpp"



void read_points(const std::string& filepath, std::vector<Ponto>  &pontos, K::FT &max_x, K::FT &max_y, K::FT &min_x, K::FT &min_y) {

    std::ifstream filePontos(filepath);

    if (!filePontos.is_open()) {
        
        return;
    }

    std::string line;
    long double x, y;
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

        Ponto aux_pont(x,y,id);
        pontos.push_back(aux_pont);
        tree.insert(aux_pont.point);
 
        max_x = std::max(max_x, aux_pont.point.x());
        max_y = std::max(max_y, aux_pont.point.y());
        min_x = std::min(min_x, aux_pont.point.x());
        min_y = std::min(min_y, aux_pont.point.y());
        id++;
    }
    
}
