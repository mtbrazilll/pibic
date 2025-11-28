#ifndef SEIP_H
#define SEIP_H

#pragma once

#include "Componentes.hpp"
#include "Struct.h"
#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

extern double bsf;
extern std::vector<Ponto>
    pontos; // Vetor global com todos os elementos a serem cobertos

// -------------------------------------------------
// Classe BitSet para manipulação eficiente de conjuntos
// -------------------------------------------------

class BitSet {
private:
  std::vector<uint64_t> bits;
  size_t size_;

public:
  // Construtor para um bitset de tamanho específico (todos os bits em 0)
  BitSet(size_t size) : size_(size) { bits.resize((size + 63) / 64, 0); }

  // Construir a partir de um vector<bool>
  BitSet(const std::vector<bool> &vec) : size_(vec.size()) {
    bits.resize((vec.size() + 63) / 64, 0);
    for (size_t i = 0; i < vec.size(); ++i) {
      if (vec[i]) {
        set(i);
      }
    }
  }

  // Definir um bit específico como 1
  inline void set(size_t pos) {
    // if (pos < size_) { // Removido check de bounds para performance
    bits[pos / 64] |= (1ULL << (pos % 64));
    // }
  }

  // Definir um bit específico como 0
  inline void reset(size_t pos) {
    // if (pos < size_) {
    bits[pos / 64] &= ~(1ULL << (pos % 64));
    // }
  }

  // Inverter um bit específico
  inline void flip(size_t pos) {
    // if (pos < size_) {
    bits[pos / 64] ^= (1ULL << (pos % 64));
    // }
  }

  // Verificar se um bit está setado
  inline bool test(size_t pos) const {
    // if (pos < size_) {
    return (bits[pos / 64] & (1ULL << (pos % 64))) != 0;
    // }
    // return false;
  }

  // Contar bits setados (população)
  size_t count() const {
    size_t count = 0;
    for (uint64_t block : bits) {
      count +=
          __builtin_popcountll(block); // Função intrínseca para contar bits
    }
    return count;
  }

  // Iterar eficientemente sobre os bits setados
  template <typename Func> void for_each_set_bit(Func func) const {
    for (size_t i = 0; i < bits.size(); ++i) {
      uint64_t block = bits[i];
      if (block == 0)
        continue;

      size_t base_idx = i * 64;
      while (block != 0) {
        int bit_idx = __builtin_ctzll(block); // Count trailing zeros
        size_t real_idx = base_idx + bit_idx;
        if (real_idx < size_) {
          func(real_idx);
        }
        block &= ~(1ULL << bit_idx); // Clear the least significant bit set
      }
    }
  }

  // Operação OR bitwise
  BitSet operator|(const BitSet &other) const {
    BitSet result(size_);
    size_t min_size = std::min(bits.size(), other.bits.size());
    for (size_t i = 0; i < min_size; ++i) {
      result.bits[i] = bits[i] | other.bits[i];
    }
    return result;
  }

  // Operação AND bitwise
  BitSet operator&(const BitSet &other) const {
    BitSet result(size_);
    size_t min_size = std::min(bits.size(), other.bits.size());
    for (size_t i = 0; i < min_size; ++i) {
      result.bits[i] = bits[i] & other.bits[i];
    }
    return result;
  }

  // Operação XOR bitwise
  BitSet operator^(const BitSet &other) const {
    BitSet result(size_);
    size_t min_size = std::min(bits.size(), other.bits.size());
    for (size_t i = 0; i < min_size; ++i) {
      result.bits[i] = bits[i] ^ other.bits[i];
    }
    return result;
  }

  // Operação NOT bitwise
  BitSet operator~() const {
    BitSet result(size_);
    for (size_t i = 0; i < bits.size(); ++i) {
      result.bits[i] = ~bits[i];
    }
    // Limpar bits extras no último bloco
    if (size_ % 64 != 0) {
      uint64_t mask = (1ULL << (size_ % 64)) - 1;
      result.bits.back() &= mask;
    }
    return result;
  }

  // OR com atribuição
  BitSet &operator|=(const BitSet &other) {
    size_t min_size = std::min(bits.size(), other.bits.size());
    for (size_t i = 0; i < min_size; ++i) {
      bits[i] |= other.bits[i];
    }
    return *this;
  }

  // AND com atribuição
  BitSet &operator&=(const BitSet &other) {
    size_t min_size = std::min(bits.size(), other.bits.size());
    for (size_t i = 0; i < min_size; ++i) {
      bits[i] &= other.bits[i];
    }
    return *this;
  }

  // Verificar se todos os bits estão setados
  bool all() const {
    if (bits.empty())
      return true;
    for (size_t i = 0; i < bits.size() - 1; ++i) {
      if (bits[i] != ~0ULL)
        return false;
    }
    // Verificar o último bloco
    if (size_ % 64 == 0) {
      return bits.back() == ~0ULL;
    } else {
      uint64_t mask = (1ULL << (size_ % 64)) - 1;
      return (bits.back() & mask) == mask;
    }
  }

  // Verificar se algum bit está setado
  bool any() const {
    for (uint64_t block : bits) {
      if (block != 0)
        return true;
    }
    return false;
  }

  // Verificar se nenhum bit está setado
  bool none() const { return !any(); }

  // Obter o tamanho
  inline size_t size() const { return size_; }

  // Converter para std::vector<bool>
  std::vector<bool> toVector() const {
    std::vector<bool> result(size_);
    for (size_t i = 0; i < size_; ++i) {
      result[i] = test(i);
    }
    return result;
  }
  bool operator==(const BitSet &other) const {
    // Para serem iguais, o tamanho e todos os bits devem ser iguais.
    // O C++ já sabe como comparar vetores.
    return (size_ == other.size_) && (bits == other.bits);
  }

  // Operador de diferença (!=)
  bool operator!=(const BitSet &other) const {
    // A diferença é simplesmente a negação da igualdade.
    return !(*this == other);
  }
};

struct SolutionInfo {
  BitSet solution;
  int cost;
  int fitness; // O mesmo que mu

  // Construtor para facilitar a criação
  SolutionInfo(BitSet sol, int c, int fit)
      : solution(std::move(sol)), cost(c), fitness(fit) {}
};

// -------------------------------------------------
// Pré-processamento da instância
// -------------------------------------------------

// Estrutura para armazenar a instância pré-processada
struct PreprocessedInstance {
  std::vector<BitSet> componentCovers; // Bits representando elementos cobertos
                                       // por cada componente
  BitSet allElements;                  // Bits representando todos os elementos

  PreprocessedInstance(const ComponentManager &manager)
      : allElements(pontos.size()) {

    // Inicializar bitsets para cada componente
    componentCovers.reserve(manager.components.size());
    for (size_t i = 0; i < manager.components.size(); ++i) {
      BitSet cover(pontos.size());
      for (int idx : manager.components[i].pontos_indices) {
        cover.set(idx);
      }
      componentCovers.push_back(std::move(cover));
    }

    // Marcar todos os elementos como cobertos
    for (size_t i = 0; i < pontos.size(); ++i) {
      allElements.set(i);
    }
  }
};

// -------------------------------------------------
// Funções auxiliares para o problema de cobertura de conjuntos
// -------------------------------------------------

/**
 * Verifica quais elementos estão cobertos por uma solução
 */
BitSet getCoveredElements(const BitSet &solution,
                          const PreprocessedInstance &instance) {
  BitSet covered(pontos.size());

  // Otimização: iterar apenas sobre os bits setados na solução
  solution.for_each_set_bit(
      [&](size_t i) { covered |= instance.componentCovers[i]; });

  return covered;
}

/**
 * Verifica se uma solução é viável (cobre todos os elementos)
 */
bool isFeasible(const BitSet &solution, const PreprocessedInstance &instance) {
  BitSet covered = getCoveredElements(solution, instance);
  return (covered & instance.allElements) == instance.allElements;
}

/**
 * Calcula o custo de uma solução (número de conjuntos utilizados)
 */
inline int calculateCost(const BitSet &solution) { return solution.count(); }

// -------------------------------------------------
// Algoritmo Guloso otimizado com operações bitwise
// -------------------------------------------------

/**
 * Encontra o melhor próximo conjunto para adicionar à solução
 * (o que cobre mais elementos não cobertos)
 */
int getBestNextSet(const PreprocessedInstance &instance,
                   const BitSet &alreadyCovered) {
  int max_index = -1;
  int max_uncovered = -1;

  // Para cada conjunto disponível
  for (size_t i = 0; i < instance.componentCovers.size(); ++i) {
    // Elementos que este conjunto cobre e ainda não estão cobertos
    // Otimização: verificar primeiro se há interseção relevante
    // BitSet newCover = instance.componentCovers[i] & (~alreadyCovered);
    // int uncovered_count = newCover.count();

    // Versão mais direta pode ser implementada se BitSet tiver suporte a
    // count(A & ~B) Por enquanto mantemos a lógica, mas evitamos alocação se
    // possível. Como BitSet aloca vector, isso é custoso.
    // TODO: Implementar count_diff(A, B) -> count(A & ~B) sem alocar.

    BitSet newCover = instance.componentCovers[i] & (~alreadyCovered);
    int uncovered_count = newCover.count();

    // Atualizar o melhor conjunto se este cobre mais elementos não cobertos
    if (uncovered_count > max_uncovered) {
      max_uncovered = uncovered_count;
      max_index = i;
    }
  }

  return max_index;
}

/**
 * Torna uma solução viável utilizando o algoritmo guloso
 */
void makeSolutionFeasible(BitSet &solution,
                          const PreprocessedInstance &instance) {
  // Elementos já cobertos pela solução atual
  BitSet covered = getCoveredElements(solution, instance);

  // Enquanto não cobrir todos os elementos
  while ((covered & instance.allElements) != instance.allElements) {
    // Encontrar o melhor próximo conjunto
    int best_set = getBestNextSet(instance, covered);

    // Se não há mais conjuntos que possam ajudar, encerrar
    if (best_set == -1)
      break;

    // Adicionar o conjunto à solução
    solution.set(best_set);

    // Atualizar elementos cobertos
    covered |= instance.componentCovers[best_set];
  }
}

/**
 * Obtém uma solução usando o algoritmo guloso
 */
BitSet getGreedySolution(const PreprocessedInstance &instance) {
  // Iniciar com uma solução vazia
  BitSet solution(instance.componentCovers.size());

  // Tornar a solução viável
  makeSolutionFeasible(solution, instance);

  return solution;
}

// -------------------------------------------------
// Algoritmo SEIP otimizado com operações bitwise
// -------------------------------------------------

/**
 * Calcula o fitness (mu) de uma solução
 * Retorna o número de elementos cobertos
 */
inline int mu(const BitSet &solution, const PreprocessedInstance &instance) {
  return getCoveredElements(solution, instance).count();
}

/**
 * Verifica se a solução A é superior à solução B
 */
inline bool isSuperior(const SolutionInfo &infoA, const SolutionInfo &infoB) {
  if ((infoA.cost <= infoB.cost && infoA.fitness > infoB.fitness) ||
      (infoA.cost < infoB.cost && infoA.fitness >= infoB.fitness)) {
    return true;
  }
  return false;
}

void initializePopulation(std::vector<SolutionInfo> &population,
                          const ComponentManager &manager,
                          const PreprocessedInstance &instance) {
  // Add current solution
  BitSet currentSolution(manager.components.size());
  for (size_t i = 0; i < manager.components.size(); i++) {
    if (manager.components[i].eh_sol) {
      currentSolution.set(i);
    }
  }
  population.emplace_back(currentSolution, calculateCost(currentSolution),
                          mu(currentSolution, instance));

  // Add greedy solution
  BitSet greedySol = getGreedySolution(instance);
  population.emplace_back(greedySol, calculateCost(greedySol),
                          mu(greedySol, instance));

  // Add random solutions with varying density
  std::random_device rd;
  std::mt19937 gen(rd());
  for (int i = 0; i < 5; i++) {
    BitSet randomSol(manager.components.size());
    std::uniform_real_distribution<> dist(0.1, 0.5); // 10-50% bits set
    double density = dist(gen);

    for (size_t j = 0; j < manager.components.size(); j++) {
      if (std::uniform_real_distribution<>(0, 1)(gen) < density) {
        randomSol.set(j);
      }
    }
    makeSolutionFeasible(randomSol, instance);
    population.emplace_back(randomSol, calculateCost(randomSol),
                            mu(randomSol, instance));
  }
}

void localSearch(BitSet &solution, const PreprocessedInstance &instance) {
  bool improved = true;
  while (improved) {
    improved = false;
    // Try removing each component
    // Otimização: iterar apenas sobre os bits setados
    std::vector<size_t> componentsToRemove;
    solution.for_each_set_bit(
        [&](size_t i) { componentsToRemove.push_back(i); });

    for (size_t i : componentsToRemove) {
      solution.reset(i);
      if (isFeasible(solution, instance)) {
        improved = true; // Component was redundant
      } else {
        solution.set(i); // Restore component
      }
    }
  }
}

/**
 * Gera uma solução mutada a partir de uma solução existente
 */
BitSet mutate(const BitSet &solution) {
  BitSet mutated = solution;

  // Taxa de mutação baseada no inverso do tamanho da solução
  double mut_prob = 1.0 / solution.size();

  // Gerador de números aleatórios
  // Otimização: static thread_local para evitar recriação
  static thread_local std::random_device rd;
  static thread_local std::mt19937 gen(rd());
  std::uniform_real_distribution<> dist(0.0, 1.0);

  // Para cada bit na solução
  for (size_t i = 0; i < solution.size(); ++i) {
    if (dist(gen) < mut_prob) {
      // Inverter o bit (adicionar ou remover o conjunto)
      mutated.flip(i);
    }
  }

  return mutated;
}

/**
 * Implementação otimizada do algoritmo SEIP para o problema de cobertura de
 * conjuntos
 */
int findSEIPApproximation(ComponentManager &manager, int iterations) {
  // Pré-processar a instância
  PreprocessedInstance instance(manager);

  // Inicializar gerador de números aleatórios
  std::random_device rd;
  std::mt19937 gen(rd());

  // População de soluções
  std::vector<SolutionInfo> population;
  population.reserve(20); // Reserva espaço para evitar realocações frequentes

  initializePopulation(population, manager, instance);

  for (int i = 0; i < iterations; ++i) {
    // Selecionar uma solução aleatória da população
    std::uniform_int_distribution<int> dist(0, population.size() - 1);
    int idx = dist(gen);
    const SolutionInfo &selected = population[idx];

    // Criar uma versão mutada da solução
    BitSet mutated_bitset = mutate(selected.solution);

    // CALCULE O CUSTO E FITNESS APENAS UMA VEZ
    int mutated_cost = calculateCost(mutated_bitset);
    int mutated_fitness = mu(mutated_bitset, instance);
    SolutionInfo mutated_solution(std::move(mutated_bitset), mutated_cost,
                                  mutated_fitness);

    // Verificar dominância
    bool is_dominated = false;
    for (const auto &sol_info : population) {
      if (isSuperior(sol_info, mutated_solution)) {
        is_dominated = true;
        break;
      }
    }

    if (!is_dominated) {
      // Nova forma de remover dominados (mais eficiente)
      // Remove-erase idiom para evitar criar novo vetor
      auto it = std::remove_if(population.begin(), population.end(),
                               [&](const SolutionInfo &sol_info) {
                                 return isSuperior(mutated_solution, sol_info);
                               });

      population.erase(it, population.end());
      population.push_back(std::move(mutated_solution));
    }
  }

  BitSet final_solution(manager.components.size());
  int best_cost = std::numeric_limits<int>::max();
  bool feasible_solution_found = false;

  // 1. Primeira passagem: Encontrar a melhor solução entre as que JÁ SÃO
  // VIÁVEIS.
  for (const auto &sol : population) {
    int cost = sol.cost;
    if (cost < best_cost) {
      // Verificar viabilidade se necessário, mas assumimos que o custo reflete
      // isso ou que a população pode ter inviáveis. O código original pegava o
      // menor custo. Se a população mantém inviáveis, deveríamos checar
      // isFeasible. Assumindo que queremos a melhor viável:
      if (isFeasible(sol.solution, instance)) {
        best_cost = cost;
        final_solution = sol.solution;
        feasible_solution_found = true;
      }
    }
  }

  // Se não achou viável na população (improvável dado initializePopulation),
  // pega a melhor custo e conserta
  if (!feasible_solution_found && !population.empty()) {
    // Fallback: pega a de menor custo mesmo inviável
    best_cost = std::numeric_limits<int>::max();
    for (const auto &sol : population) {
      if (sol.cost < best_cost) {
        best_cost = sol.cost;
        final_solution = sol.solution;
      }
    }
  }

  // Final optimization with local search
  localSearch(final_solution, instance);
  makeSolutionFeasible(final_solution, instance);
  best_cost = calculateCost(final_solution);

  // IMPORTANTE: Atualizar as idades dos componentes apenas no final
  if (best_cost > bsf) {
    for (size_t i = 0; i < manager.components.size(); ++i) {
      if (manager.components[i].eh_sol) {
        manager.components[i].idade = 0;
        manager.components[i].eh_sol = true;
        manager.components[i].vida++;
      } else {
        manager.components[i].idade++;
        manager.components[i].eh_sol = false;
        manager.components[i].vida++;
      }
    }
    return best_cost;
  }

  for (size_t i = 0; i < manager.components.size(); ++i) {
    if (final_solution.test(i)) {
      manager.components[i].idade = 0;
      manager.components[i].eh_sol = true;
      manager.components[i].vida++;
    } else {
      manager.components[i].idade++;
      manager.components[i].eh_sol = false;
      manager.components[i].vida++;
    }
  }

  return best_cost;
}

#endif