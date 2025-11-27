#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <limits>
#include <random>
#include <cmath>



// Cabeçalhos personalizados
#include "Read_data.hpp"
#include "construct.hpp"
#include "Componentes.hpp"
#include "Cplex.h"
#include "Teste.h"
#include "FASTCOVER.h"
#include "FASTCOVER-PP.h"
#include "hexa.h"
#include "Struct.h"
#include "SEIP.h"
#include "CONSTRUTIVO.h"

/* // Definições de tipos
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
typedef CGAL::Fuzzy_sphere<Traits> Fuzzy_sphere; */

using std::vector;

// Variáveis globais
int loops_with_no_improval = 0; // Controle de loops sem melhora para fugir de ótimos locais
double max_x = std::numeric_limits<double>::lowest();
double max_y = std::numeric_limits<double>::lowest();
double min_x = std::numeric_limits<double>::max();
double min_y = std::numeric_limits<double>::max();
double maior_em_modulo = std::numeric_limits<double>::lowest();
double bsf = std::numeric_limits<double>::max();
double best_solution_time = 0.0; // Tempo em que a melhor solução foi encontrada

// CMSA PARAMETERS
double computation_time_limit = 100.0;
double cplex_time_limit = 0.31;
double determinism_rate = 0.8;
double r_limit = 0;
//double cplex_time= 0.1;
int n_of_sols = 3;
int age_limit = 1;
int candidate_list_size = 10;
bool warm_start = false;
int heuristic_emphasis = 0;
bool cplex_abort = false;
bool init = false;
int algo = 0;


int loops = 0;
int seed = 1;
double raio = 1.0; // Declarado antes de ser usado
double raio2;
double raio2x4;
unsigned long int n_pon = 0;
PontoTree tree;
int profundidade = 1;
int instancia_tamanho;
double alfa = 0;
int rele = 145;

std::vector<Ponto> pontos;
std::vector<int> indices;
ComponentManager manager;
ComponentManager bestSolution;

// Gerador de números aleatórios
std::random_device rd;
std::mt19937 gen;
std::uniform_real_distribution<> distr;
std::uniform_real_distribution<> distr2(0, 1);
std::uniform_real_distribution<> distr3(1, 2);
std::uniform_int_distribution<int> int_distr(1, 2);




// Declaração de funções
void CMSA(float time_limit, int max_age);
bool testando();

double Random(double minVal, double maxVal) {
	
    double randomValue = distr2(gen); // Número entre 0 e 1
    return (minVal + randomValue * (maxVal - minVal)); // Mapeia para o intervalo [minVal, maxVal]
}

void printHelp() {
    std::cout << "USO: ./cmsa [OPÇÕES]\n\n";
    std::cout << "OPÇÕES:\n";
    std::cout << "  -h, --help            Mostra esta mensagem de ajuda\n";
    std::cout << "  -s <num>              Define a semente para o gerador de números aleatórios (padrão: 1)\n";
    std::cout << "  -max_age <num>        Define o limite de idade máxima para componentes (padrão: 1)\n";
    std::cout << "  -t <num>              Define o limite de tempo de computação em segundos (padrão: 100.0)\n";
    std::cout << "  -cpl_t <num>          Define o limite de tempo para o CPLEX em segundos (padrão: 10.0)\n";
    std::cout << "  -i <arquivo>          Define o caminho do arquivo de entrada (padrão: ../instancias/i1.txt)\n";
    std::cout << "  -r <num>              Define o raio das bolas (padrão: 1.0)\n";
    std::cout << "  -nsols <num>          Define o número de soluções a serem geradas (padrão: 3)\n";
    std::cout << "  -h_emph <0|1>         Ativa a ênfase heurística (padrão: 0)\n";
    std::cout << "  -warm_start <0|1>     Ativa o warm start (padrão: 0)\n";
    std::cout << "  -init <0|1>           Ativa a inicialização (padrão: 0)\n";
    std::cout << "  -cpl_abort <0|1>      Ativa o aborto do CPLEX (padrão: 0)\n";
    std::cout << "  -d <int>              define a profundidade (padrão: 1)\n";
}

int main(int argc, char* argv[]) {


    // Caminho padrão do arquivo
    std::string filePath = "../instancias/i1.txt";

    // Lendo argumentos da linha de comando
    for (int i = 1; i < argc; i++) { // Inicia em 1 para pular o nome do programa
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printHelp();
            return 0;
        }
        if (strcmp(argv[i], "-s") == 0) {
            if (i + 1 < argc) {
                seed = std::atoi(argv[++i]);
            } else {
                std::cerr << "Erro: Argumento para -s está faltando.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-max_age") == 0) {
            if (i + 1 < argc) {
                age_limit = std::atoi(argv[++i]);
            } else {
                std::cerr << "Erro: Argumento para -max_age está faltando.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 < argc) {
                computation_time_limit = std::atoi(argv[++i]);
            } else {
                std::cerr << "Erro: Argumento para -t está faltando.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-cpl_t") == 0) {
            if (i + 1 < argc) {
                cplex_time_limit = double(std::atoi(argv[++i]))/100;
            } else {
                std::cerr << "Erro: Argumento para -t está faltando.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-rele") == 0) {
            if (i + 1 < argc) {
                rele = (std::atoi(argv[++i]));
            } else {
                std::cerr << "Erro: Argumento para -t está faltando.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-i") == 0) {  // Flag para o caminho do arquivo
            if (i + 1 < argc) {
                filePath = argv[++i];
            } else {
                std::cerr << "Erro: Argumento para -i está faltando.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-r") == 0) {
            if (i + 1 < argc) {
                raio = std::atof(argv[++i]);
            } else {
                std::cerr << "Erro: Argumento para -r está faltando.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-nsols") == 0) {
            if (i + 1 < argc) {
                n_of_sols = std::atoi(argv[++i]);
            } else {
                std::cerr << "Erro: Argumento para -nsols está faltando.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-h_emph") == 0) {
            if (i + 1 < argc) {
               
                heuristic_emphasis = std::atoi(argv[++i]);
            } else {
                std::cerr << "Erro: Argumento para -h_emph está faltando.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-warm_start") == 0) {
            if (i + 1 < argc) {
                int val = std::atoi(argv[++i]);
                warm_start = (val == 1);
            } else {
                std::cerr << "Erro: Argumento para -warm_start está faltando.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-init") == 0) {
            if (i + 1 < argc) {
                int val = std::atoi(argv[++i]);
                init = (val == 1);
            } else {
                std::cerr << "Erro: Argumento para -warm_start está faltando.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-cpl_abort") == 0) {
            if (i + 1 < argc) {
                int val = std::atoi(argv[++i]);
                cplex_abort = (val == 1);
            } else {
                std::cerr << "Erro: Argumento para -cpl_abort está faltando.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-algo") == 0) { // Flag para verbose timing
            if (i + 1 < argc) {
                algo = std::atoi(argv[++i]);
                
            } else {
                std::cerr << "Erro: Argumento para -v está faltando (0 ou 1).\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-alfa") == 0) { // Flag para verbose timing
            if (i + 1 < argc) {
                alfa = double((std::atoi(argv[++i])))/100;
                
            } else {
                std::cerr << "Erro: Argumento para -v está faltando (0 ou 1).\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-d") == 0) { // Flag para verbose timing
            if (i + 1 < argc) {
                profundidade = std::atoi(argv[++i]);
                
            } else {
                std::cerr << "Erro: Argumento para -v está faltando (0 ou 1).\n";
                return 1;
            }
        }
        else {
            std::cerr << "Aviso: Argumento desconhecido " << argv[i] << " ignorado.\n";
        }
    }

        
    std::cout << "Caminho do arquivo: " << filePath << "\n";
    std::cout << "profundidade: " << profundidade << "\n";
    std::cout << "rele: " << rele << "\n";
    std::cout << "alfa: " << alfa << "\n";
    std::cout << "algo: " << algo << "\n";
    std::cout << "Caminho do arquivo: " << filePath << "\n";
    std::cout << "Valor de -s: " << seed << "\n";
    std::cout << "Limite de idade: " << age_limit << "\n";
    std::cout << "Limite de tempo do CPLEX: " << cplex_time_limit << "\n";
    std::cout << "Raio: " << raio << "\n";
    std::cout << "Número de soluções: " << n_of_sols << "\n";
    std::cout << "Ênfase heurística: " << heuristic_emphasis << "\n";
    std::cout << "Warm start: " << (warm_start ? "Sim" : "Não") << "\n";
    std::cout << "Abortar CPLEX: " << (cplex_abort ? "Sim" : "Não") << "\n";

    raio2 = raio * raio;
    raio2x4 = raio2 * 4;
    gen.seed(seed);
    distr = std::uniform_real_distribution<>(-raio, raio);

    read_points(filePath, pontos, max_x, max_y, min_x, min_y, maior_em_modulo);
    indices.resize(pontos.size());
    std::iota(indices.begin(), indices.end(), 0); 
    
    n_pon = pontos.size();
    tree.insert(pontos.begin(), pontos.end());
    //build_struct();
    CMSA(cplex_time_limit, age_limit);
    
    testando();
    // manager.displayComponents();

    // solve(pontos.size());
    exit(0);
}

void CMSA(float time_limit, int max_age) {
    double opt = std::numeric_limits<double>::max();
    auto total_start = std::chrono::high_resolution_clock::now();

    double construct_total = 0;
    double solve_total = 0;
    double adapt_total = 0;
    double total_duration = 0;

    if (init){
        FASTCOVER_PP ob1(pontos);
        bsf = ob1.execute();
        auto current = std::chrono::high_resolution_clock::now();
        best_solution_time = std::chrono::duration_cast<std::chrono::milliseconds>(current - total_start).count();
        //bsf = generate_solution_cgal_1(pontos, max_x , max_y , min_x , min_y );
        for (auto& component : manager.components){
            component.eh_sol = 1;
            component.idade = 0;
        }
        

    }

    if (algo == 7){
        FASTCOVER_PP ob1(pontos);
        instancia_tamanho = ob1.execute();
    }


    // ================= CMSA Loop ==========================
    while (total_duration < computation_time_limit) {
        // CONSTRUCT
        auto construct_start = std::chrono::high_resolution_clock::now();
        for (int na = 0; na < n_of_sols; na++) {

            
            if (algo ==0){

                cmsa_sbpo();
            }
            else if (algo ==1){

                dr_enhanced();
            }
             else if (algo ==2){

                generate_divise_dr_profundidade(pontos, max_x , max_y , min_x , min_y, profundidade);
            }
             else if (algo ==3){

                cmsa_dr_particao();
                
            }
             else if (algo ==4){

                fastCover();
            }
            else if (algo == 5){

                if (loops % 2 == 1){
                    generate_divise_dr_profundidade(pontos, max_x , max_y , min_x , min_y, profundidade);
                }
                else{
                     cmsa_sbpo();
                }
            }
            else if (algo == 6){

                int aux_solution2 = generate_solution_guloso(pontos, max_x , max_y , min_x , min_y, alfa);
            }
            else if (algo == 7){

                if (instancia_tamanho >= 139){
                    cmsa_sbpo();
                }
                else{
                    dr_enhanced();
                }
            }
            else if (algo == 8){
                c_brkg();
            }
            
  
        }
        auto construct_end = std::chrono::high_resolution_clock::now();
        construct_total += std::chrono::duration_cast<std::chrono::milliseconds>(construct_end - construct_start).count();

        // SOLVE
        
        auto end = std::chrono::high_resolution_clock::now();
        r_limit = computation_time_limit - std::chrono::duration_cast<std::chrono::milliseconds>(end - total_start).count();
        r_limit = r_limit/1000 ; 
        auto solve_start = std::chrono::high_resolution_clock::now();
        
        if(r_limit < 0.001){
            break;
        }
        
        double aux_solution_cplex = std::numeric_limits<double>::max();
        if (true){

            aux_solution_cplex = cplex_run();
        }
        else{
            aux_solution_cplex= findSEIPApproximation(manager,  100);
        }
            //aux_solution_cplex = cplex_run();
        
        //aux_solution_cplex= findSEIPApproximation(manager,  100);
        //        std::cout << "solucao cplex "<<aux_solution_cplex << std::endl;

        // Atualizar melhor solução se encontrada
        if (bsf > aux_solution_cplex) {
            bsf = aux_solution_cplex;
            auto current = std::chrono::high_resolution_clock::now();
            best_solution_time = std::chrono::duration_cast<std::chrono::milliseconds>(current - total_start).count();
        }
        //bsf = greedySetCover() ();
        auto solve_end = std::chrono::high_resolution_clock::now();
        solve_total += std::chrono::duration_cast<std::chrono::milliseconds>(solve_end - solve_start).count();

        // ADAPT
        auto adapt_start = std::chrono::high_resolution_clock::now();
        manager.removeOldComponents(max_age);
        auto adapt_end = std::chrono::high_resolution_clock::now();
        adapt_total += std::chrono::duration_cast<std::chrono::milliseconds>(adapt_end - adapt_start).count();


        // melhoria local

        


        auto total_end = std::chrono::high_resolution_clock::now();
        total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();
        loops++;
    }

    auto total_end = std::chrono::high_resolution_clock::now();
    total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();

    // Exibição dos resultados

    
    std::cout << "-----------------------------------\n";
    std::cout << "CONSTRUCT time: " << construct_total << "ms\n";
    std::cout << "SOLVE time: " << solve_total << "ms\n";
    std::cout << "ADAPT time: " << adapt_total << "ms\n";
    std::cout << "Total CMSA time: " << total_duration << "ms\n";

    if (testando()){
        
        std::cout << "opt: " << bsf << std::endl;
        std::cout << "-----------------------------------\n";
        std::cout << "Loops: " << loops << std::endl;
        std::cout << "Raio: " << raio << std::endl;
        std::cout << "Best solution found at: " << best_solution_time << " ms" << std::endl;
    }
    else{
        std::cout << "opt: " << 99999999 << std::endl;
        std::cout << "-----------------------------------\n";
        std::cout << "Loops: " << loops << std::endl;
        std::cout << "Raio: " << raio << std::endl;
        std::cout << "Best solution found at: " << best_solution_time << " ms" << std::endl;
    }

}

bool testando() {
    std::vector<Component> sol;

    for (const auto& c : manager.components) {
        if (c.eh_sol) {
            sol.push_back(c);
        }
    }
    std::cout << "Teste" << std::endl;
    std::cout << "pontos_vector: " << pontos.size() << std::endl;
    std::cout << "sol size: " << sol.size() << std::endl;
    Teste teste(pontos, sol);

    return teste.execute();
    //teste.writeOutput();
}



