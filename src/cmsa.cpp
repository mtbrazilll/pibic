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

// Bibliotecas do CGAL
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/Kd_tree.h>
#include <CGAL/property_map.h>
#include <CGAL/Fuzzy_sphere.h>  

// Cabeçalhos personalizados
#include "SmallestEnclosingCircle.hpp"
#include "Read_data.hpp"
#include "construct.hpp"
#include "Componentes.hpp"
#include "Cplex.h"
#include "Teste.h"
#include "Cplex_pcdp.h"
#include "FASTCOVER.h"
#include "FASTCOVER-PP.h"
#include "hexa.h"

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

// CMSA PARAMETERS
double computation_time_limit = 100.0;
double cplex_time_limit = 10.0;
double determinism_rate = 0.8;
double r_limit = 0;
int n_of_sols = 3;
int age_limit = 1;
int candidate_list_size = 10;
bool warm_start = false;
int heuristic_emphasis = 0;
bool cplex_abort = false;
bool init = false;


int loops = 0;
int seed = 1;
double raio = 1.0; // Declarado antes de ser usado
unsigned long int n_pon = 0;
Kd_tree tree;

std::vector<Ponto> pontos;
ComponentManager manager;
ComponentManager bestSolution;

// Gerador de números aleatórios
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> distr(-raio, raio);
std::uniform_real_distribution<> distr2(0, 1);

// Declaração de funções
void CMSA(float time_limit, int max_age);
void build_struct(); // Corrigido o nome da função
void k_center(std::vector<Ponto> &pontos, ComponentManager &manager, double raio);
void k_center_cgal(std::vector<Ponto> &pontos, ComponentManager &manager, double raio);
void testando();

double Random(double minVal, double maxVal) {
	
    double randomValue = distr2(gen); // Número entre 0 e 1
    return (minVal + randomValue * (maxVal - minVal)); // Mapeia para o intervalo [minVal, maxVal]
}

int main(int argc, char* argv[]) {


    // Caminho padrão do arquivo
    std::string filePath = "../instancias/i1.txt";

    // Lendo argumentos da linha de comando
    for (int i = 1; i < argc; i++) { // Inicia em 1 para pular o nome do programa
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
                cplex_time_limit = std::atoi(argv[++i]);
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
                int val = std::atoi(argv[++i]);
                heuristic_emphasis = 1;
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
        else {
            std::cerr << "Aviso: Argumento desconhecido " << argv[i] << " ignorado.\n";
        }
    }

        // Exemplo de uso das variáveis (substitua pelo processamento real)
    std::cout << "Caminho do arquivo: " << filePath << "\n";
    std::cout << "Valor de -s: " << seed << "\n";
    std::cout << "Limite de idade: " << age_limit << "\n";
    std::cout << "Limite de tempo do CPLEX: " << cplex_time_limit << "\n";
    std::cout << "Raio: " << raio << "\n";
    std::cout << "Número de soluções: " << n_of_sols << "\n";
    std::cout << "Ênfase heurística: " << heuristic_emphasis << "\n";
    std::cout << "Warm start: " << (warm_start ? "Sim" : "Não") << "\n";
    std::cout << "Abortar CPLEX: " << (cplex_abort ? "Sim" : "Não") << "\n";

    gen.seed(seed);

    read_points(filePath, pontos, max_x, max_y, min_x, min_y, maior_em_modulo);

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
        for (auto& component : manager.components){
            component.eh_sol = 1;
            component.idade = 0;
        }
        

    }


    // ================= CMSA Loop ==========================
    while (total_duration < computation_time_limit) {
        // CONSTRUCT
        auto construct_start = std::chrono::high_resolution_clock::now();
        for (int na = 0; na < n_of_sols; na++) {
           int aux_solution = generate_solution_cgal(pontos, max_x , max_y , min_x , min_y );
        //std::cout << "solucao construtivo "<<aux_solution << std::endl;
           //if (bsf > aux_solution) bsf = aux_solution;
          // mateus_recursive(pontos, max_x + raio, max_y + raio, min_x - raio, min_y -raio);
           // FASTCOVER ob2(pontos);

          // FASTCOVER_PP ob1(pontos);
           // ob2.execute();
          //ob1.execute();
            //hexa ob3(pontos);
            //ob3.execute();
            //k_center(pontos, manager, raio);
        }
        auto construct_end = std::chrono::high_resolution_clock::now();
        construct_total += std::chrono::duration_cast<std::chrono::milliseconds>(construct_end - construct_start).count();

        // SOLVE
        
        auto end = std::chrono::high_resolution_clock::now();
        r_limit = computation_time_limit - std::chrono::duration_cast<std::chrono::milliseconds>(end - total_start).count();
        r_limit = r_limit/1000 ; 
        auto solve_start = std::chrono::high_resolution_clock::now();
        
        double aux_solution_cplex = std::numeric_limits<double>::max();
        if(r_limit>=0.001){
            aux_solution_cplex = cplex_run();
        }
       
        //        std::cout << "solucao cplex "<<aux_solution_cplex << std::endl;

        if (bsf > aux_solution_cplex) bsf = aux_solution_cplex;
      //bsf = guloso();
        auto solve_end = std::chrono::high_resolution_clock::now();
        solve_total += std::chrono::duration_cast<std::chrono::milliseconds>(solve_end - solve_start).count();

        // ADAPT
        auto adapt_start = std::chrono::high_resolution_clock::now();
        manager.removeOldComponents(max_age);
        auto adapt_end = std::chrono::high_resolution_clock::now();
        adapt_total += std::chrono::duration_cast<std::chrono::milliseconds>(adapt_end - adapt_start).count();


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
    std::cout << "opt: " << bsf << std::endl;
    std::cout << "-----------------------------------\n";
    std::cout << "Loops: " << loops << std::endl;
    std::cout << "Raio: " << raio << std::endl;
}

void testando() {
    std::vector<Component> sol;

    for (const auto& c : manager.components) {
        if (c.idade == 0) {
            sol.push_back(c);
        }
    }

    std::cout << "pontos_vector: " << pontos.size() << std::endl;
     std::cout << "sol size: " << sol.size() << std::endl;
    Teste teste(pontos, sol);

    if (teste.execute())
        std::cout << "success" << std::endl;
    teste.writeOutput();
}

void build_struct() {
    tree.insert(pontos.begin(), pontos.end());
    double d = 2 * raio;  // Limite de distância (2*r)

    // Para cada ponto, encontrar seus vizinhos dentro da distância d
    for (size_t i = 0; i < pontos.size(); ++i) {
        Ponto& p = pontos[i];

        // Criar uma esfera fuzzy centrada em p com raio d
        Fuzzy_sphere sphere(p, d);

        // Coletar vizinhos dentro da distância d
        std::vector<Ponto> neighbors;
        tree.search(std::back_inserter(neighbors), sphere);

        // Para cada vizinho, armazenar o índice
        for (const Ponto& neighbor : neighbors) {
            if (neighbor.indice != p.indice) {
                p.neighbors.push_back(neighbor.indice);
            }
        }
    }

    // Exemplo: imprimir os vizinhos de cada ponto
    for (const Ponto& p : pontos) {
       // std::cout << "Ponto " << p.indice << " tem vizinhos: ";
        for (unsigned long int neighbor_index : p.neighbors) {
            //std::cout << neighbor_index << " ";
        }
       // std::cout << std::endl;
    }
    
}

/* void k_center(std::vector<Ponto> &pontos, ComponentManager &manager, double raio) {
    
    std::vector<bool> visitados(pontos.size(), false);
    unsigned int qnt_visitados = 0;

    // Enquanto existirem pontos não visitados
    while (qnt_visitados < pontos.size()) {
        // Escolhe um ponto aleatório não visitado
        unsigned long int idx = static_cast<unsigned long int>(Random(0, pontos.size() - 1));
        
        // Garantir que idx esteja dentro dos limites
        while (visitados[idx]) {
            idx = (idx + 1) % pontos.size();
        }

        Ponto& p = pontos[idx];

        // Gera um deslocamento aleatório dentro do raio
        double angle = Random(0, 2 * M_PI);
        double distance = Random(0, raio); // Distância deve ser positiva
        double dx = distance * cos(angle);
        double dy = distance * sin(angle);

        // Novo centro deslocado
        Point novo_centro(p.point.x() + dx, p.point.y() + dy);

        // Encontrar vizinhos dentro da distância raio
        std::vector<Ponto> cobertos;

        // Adicionar o próprio ponto se não visitado
        if (!visitados[p.indice]) {
            double dist = sqrt(CGAL::squared_distance(novo_centro, p.point));
            if (dist <= raio) {
                cobertos.push_back(p);
            }
        }

        // Iterar sobre os vizinhos
        for (unsigned long int neighbor_idx : p.neighbors) {
            Ponto& q = pontos[neighbor_idx];
        
            double dist = sqrt(CGAL::squared_distance(novo_centro, q.point));
            if (dist <= raio) {
                cobertos.push_back(q);
            }
            
        }

        // Cria um novo componente com o centro deslocado e pontos cobertos
        Component aux(raio, cobertos, novo_centro);
        manager.addComponent(aux);

        // Marca os pontos cobertos como visitados
        for (const auto& cp : cobertos) {
            if (!visitados[cp.indice]) {
                visitados[cp.indice] = true;
                qnt_visitados++;
            }
        }
    }
} */

 void k_center(std::vector<Ponto> &pontos, ComponentManager &manager, double raio) {
    unsigned int n_nao_visitados = pontos.size();
    std::vector<unsigned int> indices_nao_visitados(pontos.size());
    std::vector<unsigned int> indice_em_nao_visitados(pontos.size());

    // Inicializa indices_nao_visitados e indice_em_nao_visitados
    for (unsigned int i = 0; i < pontos.size(); ++i) {
        indices_nao_visitados[i] = i;
        indice_em_nao_visitados[i] = i;
    }

    // Enquanto houver pontos não visitados
    while (n_nao_visitados > 0) {
        // Escolhe um índice aleatório entre 0 e n_nao_visitados - 1
        unsigned int indice = static_cast<unsigned int>(Random(0, n_nao_visitados - 1));

        unsigned int indice_ponto = indices_nao_visitados[indice];
        Ponto &p = pontos[indice_ponto];

        // Troca o índice selecionado com o último índice não visitado
        std::swap(indices_nao_visitados[indice], indices_nao_visitados[n_nao_visitados - 1]);
        // Atualiza o mapeamento
        indice_em_nao_visitados[indices_nao_visitados[indice]] = indice;
        indice_em_nao_visitados[indices_nao_visitados[n_nao_visitados - 1]] = n_nao_visitados - 1;

        // Diminui o número de pontos não visitados
        n_nao_visitados--;

        // Gera um deslocamento aleatório dentro do raio
        double angulo = Random(0, 2 * M_PI);
        double distancia = Random(0, raio);
        double dx = distancia * cos(angulo);
        double dy = distancia * sin(angulo);

        // Novo centro deslocado
        Point novo_centro(p.point.x() + dx, p.point.y() + dy);

        // Encontra vizinhos dentro do raio
        std::vector<Ponto> cobertos;
        cobertos.push_back(p);

        for (auto indice_vizinho : p.neighbors) {
            Ponto &q = pontos[indice_vizinho];
            double dist = sqrt(CGAL::squared_distance(novo_centro, q.point));
            if (dist <= raio) {
                cobertos.push_back(q);

                // Remove o vizinho de indices_nao_visitados se ainda não foi visitado
                unsigned int indice_para_remover = indice_em_nao_visitados[indice_vizinho];
                if (indice_para_remover < n_nao_visitados) {
                    std::swap(indices_nao_visitados[indice_para_remover], indices_nao_visitados[n_nao_visitados - 1]);
                    // Atualiza o mapeamento
                    indice_em_nao_visitados[indices_nao_visitados[indice_para_remover]] = indice_para_remover;
                    indice_em_nao_visitados[indices_nao_visitados[n_nao_visitados - 1]] = n_nao_visitados - 1;
                    n_nao_visitados--;
                }
            }
        }

        // Cria um novo componente com o centro deslocado e pontos cobertos
        Component aux(raio, cobertos, novo_centro);
        manager.addComponent(aux);

        // Todos os pontos cobertos foram marcados como visitados
    }
} 
 

/* 
 void k_center(std::vector<Ponto> &pontos, ComponentManager &manager, double raio) {
    unsigned int n_unvisited = pontos.size();
    
    std::vector<unsigned int> unvisited_indices(pontos.size());
    for (unsigned int i = 0; i < pontos.size(); ++i)
        unvisited_indices[i] = i;

    // Enquanto existirem pontos não visitados
    while (n_unvisited > 0) {
        // Escolhe um índice aleatório entre 0 e n_unvisited - 1
        unsigned long int idx = int(Random(0, n_unvisited - 1));

        unsigned int point_idx = unvisited_indices[idx];
        Ponto &p = pontos[point_idx];

        // Swap o índice selecionado com o último índice não visitado
        std::swap(unvisited_indices[idx], unvisited_indices[n_unvisited - 1]);

        // Decrease the number of unvisited points
        n_unvisited--;

        // Gera um deslocamento aleatório dentro do raio
        double angle = Random(0, 2 * M_PI);
        double distance = Random(0, raio);
        double dx = distance * cos(angle);
        double dy = distance * sin(angle);

        // Novo centro deslocado
        Point novo_centro(p.point.x() + dx, p.point.y() + dy);

        // Encontrar vizinhos dentro da distância raio
        std::vector<Ponto> cobertos;
        cobertos.push_back(p);

        for (auto neighbor_idx : p.neighbors) {
            Ponto &q = pontos[neighbor_idx];
            double dist = sqrt(CGAL::squared_distance(novo_centro, q.point));
            if (dist <= raio) {
                cobertos.push_back(q);
            }
        }

        // Cria um novo componente com o centro deslocado e pontos cobertos
        Component aux(raio, cobertos, novo_centro);
        manager.addComponent(aux);

        // marque os pontos cobertos como visitados
    }
}  */
/*
void k_center_cgal(std::vector<Ponto> &pontos, ComponentManager &manager, double raio) {
    
    std::vector<bool> visitados(pontos.size(), false);
    unsigned int qnt_visitados = 0;
    // Enquanto existirem pontos não visitados
    while (qnt_visitados < pontos.size()) {
        
        //gera um ponto aleatorio p

       
       Fuzzy_sphere sphere(p, raio);
       // Coletar vizinhos dentro da distância d
        std::vector<Ponto> cobertos;
        tree.search(std::back_inserter(cobertos), sphere);

        // Cria um novo componente com o centro deslocado e pontos cobertos
        Component aux(raio,cobertos,p);
		manager.addComponent(aux);


        // Marca os pontos cobertos como visitados
        for (const auto& cp : cobertos) {
            if (!visitados[cp.indice]) {
                visitados[cp.indice] = true;
                qnt_visitados++;
            }
        }
    }
}*/

/* void k_center(std::vector<Ponto> &pontos, ComponentManager &manager, double raio) {
    
    std::vector<bool> visitados(pontos.size(), false);
    unsigned int qnt_visitados = 0;

    // Enquanto existirem pontos não visitados
    while (qnt_visitados < pontos.size()) {
        // Escolhe um ponto aleatório não visitado
        unsigned long int idx = static_cast<unsigned long int>(Random(0, pontos.size() - 1));
        
        // Garantir que idx esteja dentro dos limites
        while (visitados[idx]) {
            idx = (idx + 1) % pontos.size();
        }

        Ponto& p = pontos[idx];

        // Gera um deslocamento aleatório dentro do raio
        double angle = Random(0, 2 * M_PI);
        double distance = Random(0, raio); // Distância deve ser positiva
        double dx = distance * cos(angle);
        double dy = distance * sin(angle);

        // Novo centro deslocado
        Point novo_centro(p.point.x() + dx, p.point.y() + dy);

        // Encontrar vizinhos dentro da distância raio
        std::vector<Ponto> cobertos;

        // Adicionar o próprio ponto se não visitado
        if (!visitados[p.indice]) {
            double dist = sqrt(CGAL::squared_distance(novo_centro, p.point));
            if (dist <= raio) {
                cobertos.push_back(p);
            }
        }

        // Iterar sobre os vizinhos
        for (unsigned long int neighbor_idx : p.neighbors) {
            Ponto& q = pontos[neighbor_idx];
            if (!visitados[q.indice]) {
                double dist = sqrt(CGAL::squared_distance(novo_centro, q.point));
                if (dist <= raio) {
                    cobertos.push_back(q);
                }
            }
        }

        // Cria um novo componente com o centro deslocado e pontos cobertos
        Component aux(raio, cobertos, novo_centro);
        manager.addComponent(aux);

        // Marca os pontos cobertos como visitados
        for (const auto& cp : cobertos) {
            if (!visitados[cp.indice]) {
                visitados[cp.indice] = true;
                qnt_visitados++;
            }
        }
    }
}  */

