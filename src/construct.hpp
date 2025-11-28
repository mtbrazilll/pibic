#pragma once
#include <cmath>
#include <random>
#include <stack>
#include <iostream>
#include <ctime>
#include <queue>
#include "Componentes.hpp"
#include <limits> // Para obter os valores de limite de double


#include "FASTCOVER.h"
#include "FASTCOVER-PP.h"
#include <algorithm> // For std::nth_element
#include "Struct.h"
#include "DistanceCache.h"





using namespace std;

// Definições de tipos



extern IndexTree tree;

extern ComponentManager manager;
extern int seed;
extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<>distr2;
extern std::uniform_int_distribution<int> int_distr;
extern double raio;
extern std::vector<int> indices;
extern std::vector<Ponto> pontos;
extern double raio2;
extern double raio2x4;
extern std::uniform_real_distribution<> distr3;
extern DistanceCache cache;



int generate_solution_cgal_manual(const std::vector<Ponto>& points,
                               double x_max, double y_max,
                               double x_min, double y_min);

                               
int generate_solution_cgal(const std::vector<Ponto>& points, double x_max, double y_max, double x_min, double y_min);

int construtivo_brkg(const std::vector<Ponto>& points);

int generate_solution_dr(const std::vector<Ponto>& points,
    double x_max, double y_max,
    double x_min, double y_min);

int generate_solution_cgal_1(const std::vector<Ponto>& points,
                               double x_max, double y_max,
                               double x_min, double y_min);

int generate_solution_dr_enhanced(const std::vector<int>& point_indices);



int generate_divise_dr(const std::vector<Ponto>& points,
                               double x_max, double y_max,
                               double x_min, double y_min);



int generate_divise_dr_profundidade(const std::vector<Ponto> &points,
                       double x_max, double y_max,
                       double x_min, double y_min,
                       int max_depth);


int generate_solution_guloso(const std::vector<Ponto>& points,
                               double x_max, double y_max,
                               double x_min, double y_min);


int generate_solution_guloso(const std::vector<Ponto>& points,
                               double x_max, double y_max,
                               double x_min, double y_min, double p);