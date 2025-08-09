// arquivo_meu.hpp

#ifndef Teste_H
#define Teste_H

#include "Struct.h"
#include <vector> 
#include "Componentes.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <iostream>
#include <cmath>


typedef K::FT FT;
extern double raio;

class Teste {
   const double EPSILON = raio*raio*(1+1e-14);
   std::vector<Ponto> P;
   
   std::vector<Component> sol;

public:
    
    Teste(std::vector<Ponto> &P, std::vector<Component> &sol) : P(P), sol(sol) { }

    bool execute() {

        
        int count_pontos = P.size();
        std::vector<int> visitados(count_pontos, 0);
        bool passou = true;

        for (const auto& component : sol) {

            for (const auto& ponto: component.points)
            {
                FT distSquared = CGAL::squared_distance(ponto.point, component.pos);
                // FT raioSquared = component.raio * component.raio;

                if (distSquared > EPSILON) {
                    //std::cout << std::abs(distSquared - raioSquared) << std::endl;
                    //std::cout << "ERRO 1" << std::endl;
                    passou = false;
                }
               
              
                
               

                
                if (!visitados[ponto.indice]){
                    visitados[ponto.indice] = 1;
                    count_pontos--;
                }
            }
            
        }
        if (count_pontos != 0){
            //std::cout << "ERRO 2 " << count_pontos << std::endl;
            passou = false;
        }
        return passou;     
    }


    void writeOutput() {
        std::ofstream pointsFile("solution/points_output.txt");
        std::ofstream discsFile("solution/discs_output.txt");

        if (!pointsFile.is_open() || !discsFile.is_open()) {
            std::cerr << "Erro ao abrir os arquivos de saída." << std::endl;
            return;
        }

        int disc_id = 0; // Identificador para cada disco

        for (const auto& component : sol) {
            // Escreve informações do disco
            discsFile << component.pos.x() << " " << component.pos.y() << " " << component.raio << " " << disc_id << "\n";

            // Escreve os pontos cobertos pelo disco, associando-os ao disco
            for (const auto& ponto : component.points) {
                pointsFile << ponto.point.x() << " " << ponto.point.y() << " " << ponto.indice << " " << disc_id << "\n";
            }
            disc_id++;
        }

        pointsFile.close();
        discsFile.close();
    }
};


#endif // ARQUIVO_MEU_HP
