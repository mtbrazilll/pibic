#pragma once
#include <vector>
#include <iostream>
#include <unordered_map>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <boost/multiprecision/gmp.hpp>




#include "SmallestEnclosingCircle.hpp"

class Component {
public:
    int id;
    int idade;
    int idade_rastreio;
    int eh_sol;
    double raio;
    std::vector<Ponto> points;
    std::vector<int> points_id;
    Point pos;

    Component() 
        : id(0), idade(0), raio(0.0), idade_rastreio(0), eh_sol(0) {} 

    //Component(int id, int idade, int idade_rastreio, double raio, const std::vector<Ponto>& points, const Point& pos);

    Component(int id, double raio, const std::vector<Ponto>& points,const Point& pos)
    {   
        this->id = id;
        this->idade = 0;
        this->idade_rastreio = 0;
        this->raio = raio;
        this->points = points;
        this->pos = pos;
        this->eh_sol = 0;
    }

};

class ComponentManager {
public:
    std::map<int, Component> components;
    void addComponent(const Component& component);
    bool removeComponent(int id);
    Component* getComponent(int id);
    size_t getComponentCount() const;

    void displayComponents() const {
        for (const auto& pair : components) {
            const Component& component = pair.second;

            std::cout << "Component ID: " << component.id << std::endl;
            std::cout << "Idade: " << component.idade << std::endl;
            std::cout << "Idade: " << component.idade_rastreio << std::endl;
            std::cout << "Pos: (" << component.pos.x() << ", " << component.pos.y() << ")" << std::endl;
            std::cout << "Points:" << std::endl;
            for (const auto& point : component.points) {
                std::cout << "    (" << point.point.x() << ", " << point.point.y() << ")" << std::endl;
            }
            std::cout << "--------------------------------" << std::endl;
        }
    }
};