#include "Read_data.hpp"



void read_points(const std::string& filepath, std::vector<Ponto>  &pontos, K::FT &max_x, 
                K::FT &max_y, K::FT &min_x, K::FT &min_y,  K::FT &maior_em_modulo) {

    std::ifstream filePontos(filepath);

    if (!filePontos.is_open()) {
        std::cout << "Erro ao abrir o arquivo" << std::endl;
        exit(1);
    }
    if(filePontos.fail()){
        std::cout << "Arquivo nao existe" << std::endl;
        exit(1);
    }

    std::string line;
    double x, y;
    max_x = std::numeric_limits<K::FT>::lowest();
    max_y = std::numeric_limits<K::FT>::lowest();
    min_x = std::numeric_limits<K::FT>::max();
    min_y = std::numeric_limits<K::FT>::max();
    unsigned long int id = 0;
    while (getline(filePontos, line)) {
        std::istringstream iss(line);


        if (!(iss >> x >> y)) {
            std::cout << "formato de dados errado" << std::endl;
            exit(1);
        }

        Ponto aux_pont(x,y,id);
        id++;
        
        pontos.push_back(aux_pont);
        //tree.insert(aux_pont);
 
        max_x = std::max(max_x, aux_pont.point.x());
        max_y = std::max(max_y, aux_pont.point.y());
        min_x = std::min(min_x, aux_pont.point.x());
        min_y = std::min(min_y, aux_pont.point.y());

        maior_em_modulo = std::max(maior_em_modulo, fabs(aux_pont.point.x()));
        maior_em_modulo = std::max(maior_em_modulo, fabs(aux_pont.point.y()));
        
    }
    
}
