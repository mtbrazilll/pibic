// arquivo_meu.hpp

#ifndef Teste_H
#define Teste_H

#include "SmallestEnclosingCircle.hpp"

#include <vector> 
#include "Componentes.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/squared_distance_2.h>
#include <iostream>
#include <cmath>


typedef K::FT FT;

extern std::unordered_map<Point, std::size_t> pointToIndex;

class Teste {
   const double EPSILON = 1.0 + 1e-14;
   std::vector<Point> P;
  
   std::vector<Component> sol;

public:
    
    Teste(std::vector<Point> &P, std::vector<Component> &sol) : P(P), sol(sol) { }

    bool execute() {

        
        int count_pontos = P.size();
        std::vector<int> visitados(count_pontos, 0);

        for (const auto& component : sol) {

            for (const auto& ponto: component.points)
            {
                FT distSquared = CGAL::squared_distance(ponto, component.pos);
                FT raioSquared = component.raio * component.raio;

                if (CGAL::abs(distSquared - raioSquared) >= EPSILON) {
                    std::cout << "ERRO 1" << std::endl;
                }
               
              
                auto it = pointToIndex.find(ponto);
                if (it == pointToIndex.end()) std::cout << "erro" << std::endl;

                int id = it->second;
                if (!visitados[id]){
                    visitados[id] = 1;
                    count_pontos--;
                }
            }
            
        }
        if (count_pontos != 0){
            std::cout << "ERRO " << count_pontos << std::endl;
            return false;
        }
        return true;     
    }


    void writeOutput() {
        //std::ofstream pointsFile("solution/points_output.txt");
        std::ofstream discsFile("solution/discs_output.txt");

        
        for (const auto& component : sol) {
            for (const auto& point : component.points) {
                
               // pointsFile << point.x << " " << point.y << " " << point.id << "\n";
                    
                
            }

            discsFile << component.pos.x() << " " << component.pos.y() << " "
                      << component.raio << "\n";
        }

        //pointsFile.close();
        discsFile.close();
    }
};


#endif // ARQUIVO_MEU_HP
