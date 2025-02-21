#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <unordered_map>
#include "Struct.h"



void read_points(const std::string& filepath, std::vector<Ponto> &pontos, K::FT &max_x, 
                K::FT &max_y, K::FT &min_x, K::FT &min_y,  K::FT &maior_em_modulo);
