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

using std::vector;

// Variáveis globais
int loops_with_no_improval = 0; // Controle de loops sem melhora para fugir de ótimos locais
double max_x = std::numeric_limits<double>::lowest();
double max_y = std::numeric_limits<double>::lowest();
double min_x = std::numeric_limits<double>::max();
double min_y = std::numeric_limits<double>::max();
double maior_em_modulo = std::numeric_limits<double>::lowest();
double bsf = std::numeric_limits<double>::max();

int loops = 0;
int seed = 1;
int ita_construtivo = 3;
double raio = 1.0; // Declarado antes de ser usado
unsigned long int n_pon = 0;
Kd_tree tree;

std::vector<Ponto> pontos;
ComponentManager manager;

// Gerador de números aleatórios
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> distr(-raio * std::sqrt(2), raio * std::sqrt(2));
std::uniform_real_distribution<> distr2(0, 1);

// Declaração de funções
void CMSA(float time_limit, int max_age, int max_loops);
void build_struct(); // Corrigido o nome da função
void k_center(std::vector<Ponto> &pontos, ComponentManager &manager, double raio);
void k_center_cgal(std::vector<Ponto> &pontos, ComponentManager &manager, double raio);
void testando();

double Random(double minVal, double maxVal) {
	
    double randomValue = distr2(gen); // Número entre 0 e 1
    return (minVal + randomValue * (maxVal - minVal)); // Mapeia para o intervalo [minVal, maxVal]
}

int main(int argc, char* argv[]) {
    int max_age = 1, max_loops = 10;
    float time_limit = 0;

    // Caminho padrão do arquivo
    std::string filePath = "../instancias/i1.txt";

    // Lendo argumentos da linha de comando
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-s")) {
            sscanf(argv[i + 1], "%d", &seed);
            i++;
        }
        if (!strcmp(argv[i], "-a")) {
            sscanf(argv[i + 1], "%d", &max_age);
            i++;
        }
        if (!strcmp(argv[i], "-l")) {
            sscanf(argv[i + 1], "%d", &max_loops);
            i++;
        }
        if (!strcmp(argv[i], "-f")) {  // Flag para o caminho do arquivo
            filePath = argv[i + 1];
            i++;
        }
        if (!strcmp(argv[i], "-r")) {
            sscanf(argv[i + 1], "%lf", &raio);
            i++;
        }
        if (!strcmp(argv[i], "-c")) {
            sscanf(argv[i + 1], "%d", &ita_construtivo);
            i++;
        }
    }

    gen.seed(seed);

    read_points(filePath, pontos, max_x, max_y, min_x, min_y, maior_em_modulo);

    n_pon = pontos.size();
    build_struct();
    CMSA(time_limit, max_age, max_loops);
    
    testando();
    // manager.displayComponents();

    // solve(pontos.size());
    exit(0);
}

void CMSA(float time_limit, int max_age, int max_loops) {
    double opt = std::numeric_limits<double>::max();
    auto total_start = std::chrono::high_resolution_clock::now();

    double construct_total = 0;
    double solve_total = 0;
    double adapt_total = 0;
    double total_duration = 0;

    // ================= CMSA Loop ==========================
    while (total_duration < max_loops) {
        // CONSTRUCT
        auto construct_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ita_construtivo; i++) {
            //mateus(pontos, max_x + raio, max_y + raio, min_x - raio, min_y - raio);
            // FASTCOVER ob2(pontos);
            // FASTCOVER_PP ob1(pontos);
            // ob2.execute();
            // ob1.execute();
            k_center(pontos, manager, raio);
        }
        auto construct_end = std::chrono::high_resolution_clock::now();
        construct_total += std::chrono::duration_cast<std::chrono::milliseconds>(construct_end - construct_start).count();

        // SOLVE
        auto solve_start = std::chrono::high_resolution_clock::now();
        bsf = Exato_h();
        auto solve_end = std::chrono::high_resolution_clock::now();
        solve_total += std::chrono::duration_cast<std::chrono::milliseconds>(solve_end - solve_start).count();

        // ADAPT
        auto adapt_start = std::chrono::high_resolution_clock::now();
        manager.removeOldComponents(max_age);
        auto adapt_end = std::chrono::high_resolution_clock::now();
        adapt_total += std::chrono::duration_cast<std::chrono::milliseconds>(adapt_end - adapt_start).count();

        loops++;
        auto total_end = std::chrono::high_resolution_clock::now();
        total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();
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


void k_center(std::vector<Ponto> &pontos, ComponentManager &manager, double raio) {
    
    std::vector<bool> visitados(pontos.size(), false);
    unsigned int qnt_visitados = 0;
    // Enquanto existirem pontos não visitados
    while (qnt_visitados < pontos.size()) {
        // Escolhe um ponto aleatório não visitado
        unsigned long int idx;
        idx = int(Random(0,pontos.size()-1));
        
        while (visitados[idx] == true){
            ++idx;
        }

        Ponto& p = pontos[idx];

        // Gera um deslocamento aleatório dentro do raio
        double angle = Random(0,2 * M_PI);
        double distance = Random(0,raio);
        double dx = distance * cos(angle);
        double dy = distance * sin(angle);

        // Novo centro deslocado
        Point novo_centro(p.point.x() + dx, p.point.y() + dy);



        // Encontrar vizinhos dentro da distância raio
        std::vector<Ponto> cobertos;

        cobertos.push_back(p);

        for (auto neighbor_idx : p.neighbors) {
            Ponto& q = pontos[neighbor_idx];
            if (visitados[q.indice] == false) {
                double dist = sqrt(CGAL::squared_distance(novo_centro, q.point));
                if (dist <= raio) {
                    cobertos.push_back(q);
                }
            }
        }

        // Cria um novo componente com o centro deslocado e pontos cobertos
        Component aux(raio,cobertos,novo_centro);
		manager.addComponent(aux);


        // Marca os pontos cobertos como visitados
        for (const auto& cp : cobertos) {
            if (!visitados[cp.indice]) {
                visitados[cp.indice] = true;
                qnt_visitados++;
            }
        }
    }
}

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
}