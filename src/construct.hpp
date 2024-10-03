#pragma once
#include <cmath>
#include <random>
#include <stack>
#include <iostream>
#include <ctime>
#include <queue>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include "SmallestEnclosingCircle.hpp"
#include "Componentes.hpp"
#include <limits> // Para obter os valores de limite de double
#include <CGAL/Kd_tree.h>
#include <CGAL/Search_traits_2.h>
#include "FASTCOVER.h"
#include "FASTCOVER-PP.h"
#include <algorithm> // For std::nth_element

using namespace std;

static const double EPSILON = 1+1e-12;


typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_2 Point;




extern ComponentManager manager;
extern int seed;
extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<>distr2;
extern double raio;


void mateus(vector<Ponto>const &points, double x_max, double y_max, double x_min, double y_min);


void mateus2(vector<Ponto>const &points, double x_max, double y_max, double x_min, double y_min);

void mateus3(vector<Ponto>const &points, double x_max, double y_max, double x_min, double y_min);

