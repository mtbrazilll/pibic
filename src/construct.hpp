#pragma once
#include <cmath>
#include <random>
#include <stack>
#include <iostream>
#include <ctime>
#include <queue>
#include "SmallestEnclosingCircle.hpp"
#include "Componentes.hpp"
#include <limits> // Para obter os valores de limite de double


#include "FASTCOVER.h"
#include "FASTCOVER-PP.h"
#include <algorithm> // For std::nth_element


// Bibliotecas do CGAL
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/Kd_tree.h>
#include <CGAL/property_map.h>
#include <CGAL/Fuzzy_sphere.h>  
#include <CGAL/squared_distance_2.h>
#include <CGAL/Search_traits_2.h>



// Definições de tipos
typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_2 Point;

// Mapa de propriedade para acessar o ponto de Ponto
struct Ponto_to_Point_map {
    typedef Ponto key_type;
    typedef Point value_type;
    typedef const value_type& reference;
    typedef boost::readable_property_map_tag category;

    friend inline reference get(const Ponto_to_Point_map&, const key_type& k) {
        return k.point;
    }
};

typedef Ponto_to_Point_map Point_map;
typedef CGAL::Search_traits_2<K> Base_traits;
typedef CGAL::Search_traits_adapter<Ponto, Point_map, Base_traits> Traits;
typedef CGAL::Kd_tree<Traits> Kd_tree;
typedef CGAL::Fuzzy_sphere<Traits> Fuzzy_sphere;



using namespace std;

static const double EPSILON = 1+1e-12;



extern Kd_tree tree;

extern ComponentManager manager;
extern int seed;
extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<>distr2;
extern double raio;

int generate_solution(vector<Ponto>const &points, double x_max, double y_max, double x_min, double y_min);



int generate_solution_recursive(std::vector<Ponto>& points, double x_max, double y_max, double x_min, double y_min);
void generate_solution_recursive_real(std::vector<Ponto>& points, double x_max, double y_max, double x_min, double y_min, int & value_solution);

int generate_solution_2(const std::vector<Ponto>& points, double x_max, double y_max, double x_min, double y_min);
int generate_solution_cgal(const std::vector<Ponto>& points, double x_max, double y_max, double x_min, double y_min);