#pragma once

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <unordered_map>
#include <CGAL/Kd_tree.h>
#include <CGAL/Search_traits_2.h>
#include "SmallestEnclosingCircle.hpp"

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_2 Point;




void read_points(const std::string& filepath, std::vector<Ponto> &pontos, K::FT &max_x, 
                K::FT &max_y, K::FT &min_x, K::FT &min_y,  K::FT &maior_em_modulo);
