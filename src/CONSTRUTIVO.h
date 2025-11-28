#ifndef CONSTRUTIVO_H
#define CONSTRUTIVO_H

#pragma once

#include "Componentes.hpp"
#include "Struct.h"
#include "construct.hpp"
#include "DistanceCache.h"



extern double bsf;
extern std::vector<Ponto> pontos;
extern int loops;
extern double max_x;
extern double max_y;
extern double min_x;
extern double min_y;
//DistanceCache cache;








void cmsa_sbpo(){

    if (loops == 0) {

        int aux_solution1 = generate_solution_cgal_1(pontos, max_x , max_y , min_x , min_y );
    }
    else{

        int aux_solution2 = generate_solution_cgal(pontos, max_x , max_y , min_x , min_y );

    }
}

void dr_enhanced(){
 
    std::vector<int> indices(pontos.size());
    for(size_t i=0; i<pontos.size(); ++i) indices[i] = pontos[i].indice;
    int aux_solution1 = generate_solution_dr_enhanced(indices);
}

void divide_dr(){
   
    int aux_solution1 = generate_divise_dr_profundidade(pontos, max_x , max_y , min_x , min_y, 2);
    // int aux_solution1 = generate_divise_dr(pontos, max_x , max_y , min_x , min_y);
}

void cmsa_dr_particao(){

    if (loops % 2 == 0) {

        cmsa_sbpo();
    }
    else{

        dr_enhanced();

    }
}

void fastCover(){

                //dr_enhanced();
            //cmsa_dr_particao();
            //divide_dr();
           // int aux_solution1 = generate_divise_dr(pontos, max_x , max_y , min_x , min_y );
           
          
            //int aux_solution2 = generate_solution_cgal_1(pontos, max_x , max_y , min_x , min_y );
            //int aux_solution1 = generate_solution_dr_enhanced(pontos, max_x , max_y , min_x , min_y );

           //std::cout << "solucao construtivo "<<aux_solution1 << std::endl;
           //int aux_solution2 = construtivo_brkg(pontos);
        //std::cout << "solucao construtivo "<<aux_solution << std::endl;
           //if (bsf > aux_solution) bsf = aux_solution;
          // mateus_recursive(pontos, max_x + raio, max_y + raio, min_x - raio, min_y -raio);
           // FASTCOVER ob2(pontos);

          FASTCOVER_PP ob1(pontos);
           // ob2.execute();
          ob1.execute();
            //hexa ob3(pontos);
            //ob3.execute();
            //k_center(pontos, manager, raio);
}


void c_brkg(){
     construtivo_brkg(pontos);
}
#endif



