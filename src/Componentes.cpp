#include "Componentes.hpp"


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