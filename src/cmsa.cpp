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
#include "FASTCOVER.h"

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_2 Point;
typedef CGAL::Search_traits_2<K> TreeTraits;
typedef CGAL::Kd_tree<TreeTraits> Tree;


using std::size_t;
using std::vector;

int loops_with_no_improval = 0; // controle de loops sem melhora para fugir de otimos locais
double max_x = std::numeric_limits< double>::lowest(), max_y = std::numeric_limits<double>::lowest();
double min_x = std::numeric_limits< double>::max(), min_y = std::numeric_limits<double>::max();
double maior_em_modulo = std::numeric_limits< double>::lowest();
double bsf = std::numeric_limits<double>::max();

//gerador de numeros aleatorios
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<>distr(-std::sqrt(2), std::sqrt(2));
std::uniform_real_distribution<> distr2(0, 1);


int loops = 0;
int id_aux_d = 0;
int seed = 1;
unsigned long int n_pon = 0;


std::unordered_map<Point, std::vector<int>> cellToDisks;
Tree tree;
std::vector<Ponto> pontos;
std::vector<int> idsToRemove;
ComponentManager manager;
std::vector<Component> sol_reduzida;

void CMSA(float time_limit, int max_age, int max_loops);

void testando();

int main(int argc, char *argv[]) {
    int max_age = 1, max_loops = 10;
    float time_limit = 0;   

    //std::cout <<"seed: "<< seed << std::endl;
    std::string filePath = "../instancias/i4_pon.txt";  // Default file path
 

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

    gen.seed(seed);

    read_points(filePath, pontos, max_x, max_y, min_x, min_y,maior_em_modulo); 
    n_pon = pontos.size();
    
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
    double total_duration = 0;




     //================= CMSA inicializa ==========================

    //FASTCOVER ob(pontos);

    //testando();

   

    
    //================= CMSA Loop ==========================
    //while (loops < max_loops) {
     while (total_duration < max_loops) {              
        //CONSTRUCT 
       // std::cout << "-----------------------------------\n";
       // std::cout << "---------iniciando-loop--------\n";
        auto construct_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 3; i++) {
            mateus(pontos,max_x+1,max_y+1,min_x-1,min_y-1);
            //ob.execute();
        }

        auto construct_end = std::chrono::high_resolution_clock::now();
        construct_total += std::chrono::duration_cast<std::chrono::milliseconds>(construct_end - construct_start).count();
        //std::cout << "quantidade: " << manager.getComponentCount() << std::endl;
        


        //SOLVE
        auto solve_start = std::chrono::high_resolution_clock::now();
        bsf = guloso();
        auto solve_end = std::chrono::high_resolution_clock::now();
        solve_total += std::chrono::duration_cast<std::chrono::milliseconds>(solve_end - solve_start).count();    
        //std::cout << "SOLVE total time: " << solve_total << "ms\n";
        // ADAPT
        auto adapt_start = std::chrono::high_resolution_clock::now();
        manager.removeOldComponents(max_age);
        auto adapt_end = std::chrono::high_resolution_clock::now();
        adapt_total += std::chrono::duration_cast<std::chrono::milliseconds>(adapt_end - adapt_start).count();
        //testando();
        loops++;
        //std::cout<<"otimo atual: " << bsf <<"\n";
        //std::cout << "---------Finalizando-loop--------\n";
        auto total_end = std::chrono::high_resolution_clock::now();
        total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();


    }

    auto total_end = std::chrono::high_resolution_clock::now();
    total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();

    if (true) {
        std::cout << "-----------------------------------\n";
        std::cout << "CONSTRUCT time: " << construct_total << "ms\n";
        std::cout << "SOLVE time: " << solve_total << "ms\n";
        std::cout << "ADAPT time: " << adapt_total << "ms\n";
        
        
        //std::cout << "Total CMSA time: " << total_duration.count() << "ms\n";
    }

    std::cout << "Total CMSA time: " << total_duration << "ms\n";
    std::cout << "opt: " << bsf << std::endl;
    std::cout << "-----------------------------------\n";
    std::cout<<  "Loops: " << loops << std::endl;
}


void testando(){

    std::vector<Component> sol;

    for (const auto& c : manager.components) {


        if (c.idade == 0) {
				 
			sol.push_back(c);
		}
			
	}

    std::cout<<"pontos_vector:" << pontos.size() << std::endl;
    
    Teste teste(pontos,sol);
	if (teste.execute()) std::cout << "success" << endl;
    teste.writeOutput();

}