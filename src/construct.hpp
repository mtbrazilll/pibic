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

using namespace std;

static const double EPSILON = 1+1e-12;


typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_2 Point;
typedef CGAL::Search_traits_2<K> TreeTraits;
typedef CGAL::Kd_tree<TreeTraits> Tree;


extern Tree tree;

extern vector<Point> points;
extern int loops;
extern int id_aux_d;
extern ComponentManager manager;
extern unsigned seed;

int mateus(vector<Point> &points, double x_max, double y_max, double x_min, double y_min);

