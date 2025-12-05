#pragma once
#include <algorithm>
#include <boost/multiprecision/gmp.hpp>
#include <iostream>
#include <set>
#include <vector>

#include "Struct.h"

class Component {
public:
  int idade;
  int vida;
  bool eh_sol;
  double raio;
  std::vector<int> pontos_indices; // Armazena índices dos pontos
  Point pos;

  Component() : idade(0), raio(0.0), eh_sol(false) {}

  Component(double raio, const std::vector<int> &pontos_indices,
            const Point &pos)
      : vida(0), idade(0), raio(raio), pontos_indices(pontos_indices), pos(pos),
        eh_sol(false) {}
};

class ComponentManager {
public:
  std::vector<Component> components;

  void addComponent(const Component &component) {
    components.push_back(component);
  }

  void addComponent(Component &&component) {
    components.push_back(std::move(component));
  }

  bool removeComponent(int position) {
    if (position >= 0 && position < components.size()) {
      // Swap with the last element and pop back for O(1) removal
      // This changes the order of elements, which is acceptable for this
      // metaheuristic
      if (position != components.size() - 1) {
        std::swap(components[position], components.back());
      }
      components.pop_back();
      return true;
    }
    return false;
  }

  size_t getComponentCount() const { return components.size(); }

  void removeOldComponents(int max_age) {
    // Use erase-remove idiom to avoid allocating a new vector
    components.erase(std::remove_if(components.begin(), components.end(),
                                    [max_age](const Component &temp) {
                                      return temp.idade >= max_age;
                                    }),
                     components.end());
  }

  void clear() { components.clear(); }

  void reserve(size_t n) { components.reserve(n); }
};
