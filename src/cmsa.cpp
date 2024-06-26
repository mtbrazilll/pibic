#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <limits>
#include "SmallestEnclosingCircle.hpp"
#include "Read_data.hpp"
#include "construct.hpp"
#include "Componentes.hpp"
#include "Cplex.h"
#include "Teste.h"
#include "Cplex_pcdp.h"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Kd_tree.h>
#include <CGAL/Search_traits_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Random.h>
#include <CGAL/random_convex_set_2.h>

typedef CGAL::Simple_cartesian<long double> K;
typedef K::Point_2 Point;
typedef CGAL::Search_traits_2<K> TreeTraits;
typedef CGAL::Kd_tree<TreeTraits> Tree;


using std::size_t;
using std::vector;

int loops_with_no_improval = 0; // controle de loops sem melhora para fugir de otimos locais
long double max_x = std::numeric_limits<long double>::lowest(), max_y = std::numeric_limits<long double>::lowest();
long double min_x = std::numeric_limits<long double>::max(), min_y = std::numeric_limits<long double>::max();
long double maior_em_modulo = std::numeric_limits<long double>::lowest();
double bsf = std::numeric_limits<double>::max();



int loops = 0;
int id_aux_d = 0;
unsigned seed = time(0);


std::unordered_map<Point, std::vector<int>> cellToDisks;
Tree tree;
vector<Ponto> pontos;
std::vector<int> idsToRemove;
ComponentManager manager;
std::vector<Component> sol_reduzida;

void CMSA(float time_limit, int max_age, int max_loops);

void testando();

int main(int argc, char *argv[]) {
    int max_age = 1, max_loops = 10;
    float time_limit = 0;   

    //std::cout <<"seed: "<< seed << std::endl;
    std::string filePath = "../instancias/i1.txt";  // Default file path
    srand(seed);

    // lendo argumentos da linha de comando
    for (int i = 0; i < argc; i++) {
        if(!strcmp(argv[i], "-s")) {          
            sscanf(argv[i+1], "%d", &seed);
            i++;
        }
        if(!strcmp(argv[i], "-a")) {          
            sscanf(argv[i+1], "%d", &max_age);
            i++;
        }
        if(!strcmp(argv[i], "-l")) {          
            sscanf(argv[i+1], "%d", &max_loops);
            i++;
        }
        if(!strcmp(argv[i], "-f")) {  // Add a new flag for file path         
            filePath = argv[i+1];
            i++;
        }
    }

    read_points(filePath, pontos, max_x, max_y, min_x, min_y,maior_em_modulo); 
    
    
    CMSA(time_limit, max_age, max_loops);
    testando();
    //manager.displayComponents();
    
   
    //solve(pontos.size());
    exit(0);
    
}

void CMSA(float time_limit, int max_age, int max_loops) {
    double opt = std::numeric_limits<double>::max();
    auto total_start = std::chrono::high_resolution_clock::now();

    double construct_total = 0;
    double solve_total = 0;
    double adapt_total = 0;

    //================= CMSA Loop ==========================
    while (loops < max_loops) {              
        //CONSTRUCT 
        auto construct_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 3; i++) {
            mateus(pontos, max_x+1, max_y+1, min_x-1, min_y-1);
        }

        auto construct_end = std::chrono::high_resolution_clock::now();
        construct_total += std::chrono::duration_cast<std::chrono::milliseconds>(construct_end - construct_start).count();
        //std::cout << "quantidade: " << manager.getComponentCount() << std::endl;
        


        //SOLVE
        auto solve_start = std::chrono::high_resolution_clock::now();
        bsf = Exato_h(pontos);
        auto solve_end = std::chrono::high_resolution_clock::now();
        solve_total += std::chrono::duration_cast<std::chrono::milliseconds>(solve_end - solve_start).count();    

        // ADAPT
        auto adapt_start = std::chrono::high_resolution_clock::now();
        for (const auto& pair : manager.components) {
            const Component& component = pair.second;	 
            if (component.idade >= max_age) {
                idsToRemove.push_back(component.id);
            }  
        }
        for (int id : idsToRemove) {
            manager.removeComponent(id);
        }
        idsToRemove.clear();
        auto adapt_end = std::chrono::high_resolution_clock::now();
        adapt_total += std::chrono::duration_cast<std::chrono::milliseconds>(adapt_end - adapt_start).count();

        loops++;
    }

    auto total_end = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start);

    if (true) {
        std::cout << "CONSTRUCT total time: " << construct_total << "ms\n";
        std::cout << "SOLVE total time: " << solve_total << "ms\n";
        std::cout << "ADAPT total time: " << adapt_total << "ms\n";
        std::cout << "-----------------------------------\n";
        //std::cout << "Total CMSA time: " << total_duration.count() << "ms\n";
    }

    std::cout << "Total CMSA time: " << total_duration.count() << "ms\n";
    std::cout << "opt: " << bsf << std::endl;
}


void testando(){

    std::vector<Component> sol;

    for (const auto& pair : manager.components) {

        const Component& c = pair.second;

        if (c.idade == 0) {
				 
			sol.push_back(c);
		}
			
	}

    std::cout<<"pontos_vector:" << pontos.size() << std::endl;
    
    Teste teste(pontos,sol);
	if (teste.execute()) std::cout << "success" << endl;
    teste.writeOutput();

}