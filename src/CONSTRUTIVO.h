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
DistanceCache cache;

void cmsa_sbpo(){

    if (loops == 0) {

        int aux_solution1 = generate_solution_cgal_1(pontos, max_x , max_y , min_x , min_y );
    }
    else{

        int aux_solution2 = generate_solution_cgal(pontos, max_x , max_y , min_x , min_y );

    }
}

void dr_enhanced(){
 
    
    int aux_solution1 = generate_solution_dr_enhanced(pontos);
}

void divide_dr(){
   
    int aux_solution1 = generate_divise_dr_profundidade(pontos, max_x , max_y , min_x , min_y, 2);
    // int aux_solution1 = generate_divise_dr(pontos, max_x , max_y , min_x , min_y);
}

void cmsa_dr_particao(){

    if (loops % 2 == 0) {

        int aux_solution1 = generate_solution_cgal_1(pontos, max_x , max_y , min_x , min_y );
    }
    else{

        int aux_solution2 = generate_solution_dr_enhanced(pontos);

    }
}
#endif