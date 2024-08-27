//
// Created by Ghosh, Anirban on 11/11/21.
//

#ifndef  FASTCOVER_H
#define  FASTCOVER_H

#include <chrono>
#include <list>
#include <vector>
#include <unordered_set>
#include "SmallestEnclosingCircle.hpp"
#include <random>
#include "Componentes.hpp"
#include <CGAL/Cartesian.h>
extern ComponentManager manager;


extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<>distr;
extern double raio;

class FASTCOVER {
    const std::vector<Ponto> &P;
   

    typedef std::pair<int,int> intPair;
    typedef std::unordered_set<intPair,boost::hash<intPair>> SetOfCells;
    typedef std::vector<Ponto> VectorType; // Altere para o tipo de vetor que você deseja.

    // Crie um mapa onde a chave é um intPair e o valor é um vetor.
    const double sqrt2 = std::sqrt(2);
    const double lado = raio*sqrt2;
    const double additiveFactor = lado/2;
    

    public:
    FASTCOVER(const std::vector<Ponto>& P) : P(P) { }

    void execute() {
        assert(!P.empty());



        double num1 = distr(gen);
        double num2 = distr(gen);

        //std::cout<< num1 << " " << num2<< endl;
        std::unordered_map<intPair, VectorType, boost::hash<intPair>> cellToVectorMap;

        for(const Ponto &p : P){
            //S.insert(std::make_pair(floor(p.point.x()/sqrt2),floor(p.point.y()/sqrt2)));
            int x = floor((p.point.x()+num1)/lado);
            int y = floor((p.point.y()+num2)/lado);

            cellToVectorMap[std::make_pair(x,y)].push_back(p);
            
        
        }
        for(auto& pair : cellToVectorMap){

            intPair cell = pair.first;
            std::vector<Ponto>& pontos_cobertos = pair.second;
            
            Point centro((pair.first.first*sqrt2+additiveFactor)-num1,(pair.first.second*sqrt2+additiveFactor)-num2);
        

            Component aux(raio,pontos_cobertos,centro);
            manager.addComponent(aux);
            
        } 
        //std::cout<<manager.components.size()<<std::endl;
                
    }
};

#endif //FASTCOVER_H
