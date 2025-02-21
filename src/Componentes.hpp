#pragma once
#include <vector>
#include <iostream>
#include <boost/multiprecision/gmp.hpp>
#include <set>

#include "Struct.h"

class Component {
public:
   
    int idade;
    bool eh_sol;
    double raio;
    std::vector<Ponto> points;
    Point pos;

    Component() 
        :idade(0), raio(0.0), eh_sol(false) {}

    Component( double raio, std::vector<Ponto>& points, const Point& pos)
        :  idade(1), raio(raio), points(points), pos(pos), eh_sol(false) {}
};

class ComponentManager {
public:
    std::vector<Component> components;

    void addComponent(const Component& component) {
      
        components.push_back(component);
    }


    bool removeComponent(int position) {
        if (position >= 0 && position < components.size()) {
            components.erase(components.begin() + position);
            return true;
        }
        return false;
    }

    size_t getComponentCount() const {
        return components.size();
    }

    void removeOldComponents(int max_age) {
        auto it = components.begin();
        std::vector<Component> aux;
        for (auto &temp: components) {
            if (temp.idade <= max_age) {
                aux.push_back(temp);
                
            }
        }
        components.clear();
        components = aux;
    }
};
