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






using namespace std;

// Definições de tipos



extern PontoTree tree;

extern ComponentManager manager;
extern int seed;
extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<>distr2;
extern double raio;
extern std::vector<int> indices;
extern std::vector<Ponto> pontos;




int generate_solution_cgal(const std::vector<Ponto>& points, double x_max, double y_max, double x_min, double y_min);
int construtivo_brkg(const std::vector<Ponto>& points);
int generate_solution_dr(const std::vector<Ponto>& points,
    double x_max, double y_max,
    double x_min, double y_min);