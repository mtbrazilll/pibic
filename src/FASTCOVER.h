//
// Created by Ghosh, Anirban on 11/11/21.
//

#ifndef UDC_FASTCOVER_H
#define UDC_FASTCOVER_H

#include <chrono>
#include <list>
#include <vector>
#include <unordered_set>
#include "SmallestEnclosingCircle.hpp"
#include <random>

#include <CGAL/Cartesian.h>
extern ComponentManager manager;
extern int id_aux_d;


extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<>distr;

class FASTCOVER {
    std::vector<Ponto> &P;
    std::list<Ponto> &diskCenters;

    typedef std::pair<int,int> intPair;
    typedef std::unordered_set<intPair,boost::hash<intPair>> SetOfCells;
    typedef std::vector<Ponto> VectorType; // Altere para o tipo de vetor que você deseja.

    // Crie um mapa onde a chave é um intPair e o valor é um vetor.
    const double sqrt2 = std::sqrt(2);
    const double additiveFactor = sqrt2/2;
    

    public:
    FASTCOVER(std::vector<Ponto> &P, std::list<Ponto> &diskCenters) : P(P), diskCenters(diskCenters) { }

    void execute() {
        assert(!P.empty());



        double num1 = distr(gen);
        double num2 = distr(gen);

        std::cout<< num1 << " " << num2<< endl;
        std::unordered_map<intPair, VectorType, boost::hash<intPair>> cellToVectorMap;

        for(const Ponto &p : P)
            //S.insert(std::make_pair(floor(p.point.x()/sqrt2),floor(p.point.y()/sqrt2)));
            cellToVectorMap[std::make_pair(floor((p.point.x()+num1)/sqrt2),floor((p.point.y()+num2)/sqrt2))].push_back(p);
            

        for(auto& pair : cellToVectorMap){

            intPair cell = pair.first;
            vector<Ponto>& vector = pair.second;
            Component aux(id_aux_d,0,0,1.0,vector,Ponto((pair.first.first*sqrt2+additiveFactor)-num1,(pair.first.second*sqrt2+additiveFactor)-num2));
            id_aux_d++;
            manager.addComponent(aux);
            
        } 
        return;        
    }
};

#endif //UDC_FASTCOVER_H
