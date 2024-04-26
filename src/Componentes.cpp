#include "Componentes.hpp"

Component::Component(int id, int idade, int idade_rastreio, double raio, const std::vector<Ponto>& points, const Point& pos)
    : id(id), idade(idade), idade_rastreio(idade_rastreio), raio(raio), points(points), pos(pos) {}

void ComponentManager::addComponent(const Component& component) {
    components[component.id] = component;
}

bool ComponentManager::removeComponent(int id) {
    auto it = components.find(id);
    if (it != components.end()) {
        components.erase(it);
        return true;
    }
    return false;
}

Component* ComponentManager::getComponent(int id) {
    auto it = components.find(id);
    if (it != components.end()) {
        return &(it->second);
    }
    return nullptr;
}

size_t ComponentManager::getComponentCount() const {
    return components.size();
}