#include <iostream>
#include <vector>

// Função para remover um elemento de um vector no índice especificado
void removerElementoNoIndice(std::vector<int>& vec, size_t indice) {
    if (indice < vec.size()) {
        vec.erase(vec.begin() + indice);
    } else {
        std::cerr << "Índice fora do intervalo!" << std::endl;
    }
}


