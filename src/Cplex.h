#ifndef Cplex_H
#define Cplex_H

#pragma once
#include <fstream>
#include <ilcplex/ilocplex.h>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "Componentes.hpp"
#include "Struct.h"
#include <functional>
#include <stack>
#include <unordered_map>
#include <unordered_set>

using std::function;
using std::stack;
using std::string;
using std::unordered_set;
using std::vector;

extern ComponentManager manager;
extern double bsf;
extern unsigned long int n_pon;
extern ComponentManager bestSolution;

extern bool warm_start;
extern int heuristic_emphasis;
extern bool cplex_abort;
extern double cplex_time_limit;
extern double r_limit;
extern int loops;
extern std::uniform_real_distribution<> distr2;
extern int seed;

ILOSTLBEGIN

ILOHEURISTICCALLBACK2(RandomizedRoundingCallback, IloNumVarArray, vars_x,
                      IloNum, dummy) {
  try {
    int numSets = vars_x.getSize();
    IloNumArray lpSolution(getEnv());
    getValues(lpSolution, vars_x);

    IloNumArray solucaoArredondada(getEnv(), numSets);
    std::vector<bool> is_covered(n_pon, false);
    int uncovered_count = n_pon;

    for (int j = 0; j < numSets; ++j) {
      if (distr2(gen) < lpSolution[j]) {
        solucaoArredondada[j] = 1.0;
        for (int idx : manager.components[j].pontos_indices) {
          if (!is_covered[idx]) {
            is_covered[idx] = true;
            uncovered_count--;
          }
        }
      } else {
        solucaoArredondada[j] = 0.0;
      }
    }

    if (uncovered_count > 0) {
      std::vector<int> candidatosReparo;
      candidatosReparo.reserve(numSets);
      for (int j = 0; j < numSets; ++j) {
        if (solucaoArredondada[j] < 0.5) {
          candidatosReparo.push_back(j);
        }
      }

      while (uncovered_count > 0) {
        int melhorConjunto_j = -1;
        double melhorCustoBeneficio = -1.0;

        for (int j : candidatosReparo) {
          int itensNovosCobertos = 0;
          for (int idx : manager.components[j].pontos_indices) {
            if (!is_covered[idx]) {
              itensNovosCobertos++;
            }
          }

          if (itensNovosCobertos > 0) {
            double custoBeneficio =
                (double)itensNovosCobertos; // Assuming cost 1
            if (custoBeneficio > melhorCustoBeneficio) {
              melhorCustoBeneficio = custoBeneficio;
              melhorConjunto_j = j;
            }
          }
        }

        if (melhorConjunto_j == -1)
          return;

        solucaoArredondada[melhorConjunto_j] = 1.0;
        for (int idx : manager.components[melhorConjunto_j].pontos_indices) {
          if (!is_covered[idx]) {
            is_covered[idx] = true;
            uncovered_count--;
          }
        }

        // Remove selected set from candidates (swap with last and pop)
        for (size_t k = 0; k < candidatosReparo.size(); ++k) {
          if (candidatosReparo[k] == melhorConjunto_j) {
            candidatosReparo[k] = candidatosReparo.back();
            candidatosReparo.pop_back();
            break;
          }
        }
      }
    }

    setSolution(vars_x, solucaoArredondada);
  } catch (IloException &e) {
    cerr << "Erro no HeuristicCallback: " << e << endl;
  }
}

ILOHEURISTICCALLBACK2(GreedySetCoverCallback, IloNumVarArray, vars_x, IloNum,
                      dummy) {
  if (false)
    return;

  try {
    int numSets = vars_x.getSize();
    IloNumArray lpSolution(getEnv());
    getValues(lpSolution, vars_x);

    IloNumArray solucaoGulosa(getEnv(), numSets);
    for (int j = 0; j < numSets; ++j)
      solucaoGulosa[j] = 0.0;

    std::vector<bool> is_covered(n_pon, false);
    int uncovered_count = n_pon;

    std::vector<int> candidatos;
    candidatos.reserve(numSets);
    for (int j = 0; j < numSets; ++j) {
      if (lpSolution[j] > 0.5) {
        candidatos.push_back(j);
      }
    }

    while (uncovered_count > 0) {
      int melhorConjunto_j = -1;
      int maxItensCobertos = 0;

      for (int j : candidatos) {
        int itensQueEsseConjuntoCobre = 0;
        for (int idx : manager.components[j].pontos_indices) {
          if (!is_covered[idx]) {
            itensQueEsseConjuntoCobre++;
          }
        }

        if (itensQueEsseConjuntoCobre > maxItensCobertos) {
          maxItensCobertos = itensQueEsseConjuntoCobre;
          melhorConjunto_j = j;
        }
      }

      if (melhorConjunto_j == -1)
        return;

      solucaoGulosa[melhorConjunto_j] = 1.0;

      for (int idx : manager.components[melhorConjunto_j].pontos_indices) {
        if (!is_covered[idx]) {
          is_covered[idx] = true;
          uncovered_count--;
        }
      }

      // Remove from candidates
      for (size_t k = 0; k < candidatos.size(); ++k) {
        if (candidatos[k] == melhorConjunto_j) {
          candidatos[k] = candidatos.back();
          candidatos.pop_back();
          break;
        }
      }
    }

    if (uncovered_count == 0) {
      setSolution(vars_x, solucaoGulosa);
    }

  } catch (IloException &e) {
    cerr << "Erro no HeuristicCallback: " << e << endl;
  }
}

ILOMIPINFOCALLBACK2(Callback, IloBool, aborted, IloNum, sol) {

  if (!aborted && hasIncumbent()) {

    IloNum cost = getIncumbentObjValue();

    // printf("Found solution value: %f\n", cost);

    if (cost < sol) {
      // printf("Cost is better than bsf and time limit run out. Quiting after
      // %fs...\n", timeUsed);
      aborted = IloTrue;
      abort();
    }
  }
}

ILOMIPINFOCALLBACK1(abortCallback, int, curbest) {
  if (hasIncumbent()) {
    IloNum nv = getIncumbentObjValue();
    if (curbest > int(nv)) {
      abort();
    }
  }
}

ILOSIMPLEXCALLBACK0(MyCallback) {
  std::cout << "Iteration " << getNiterations() << ": ";
  if (isFeasible()) {
    std::cout << "Objective = " << getObjValue() << endl;
  } else {
    std::cout << "Infeasibility measure = " << getInfeasibility() << endl;
  }
}

double cplex_run() {
  double solution_cplex = std::numeric_limits<double>::max();

  IloEnv env;
  env.setOut(env.getNullStream());
  env.setWarning(env.getNullStream());

  try {
    IloModel model(env);

    // Create constraints first (rows)
    IloRangeArray constraints(env, n_pon);
    for (int i = 0; i < n_pon; ++i) {
      constraints[i] = IloRange(env, 1.0, IloInfinity);
    }
    model.add(constraints);

    // Create objective
    IloObjective obj = IloMinimize(env);
    model.add(obj);

    // Create variables (columns)
    IloNumVarArray x(env);

    // Prepare warm-start data
    IloNumVarArray mipVar(env);
    IloNumArray mipVal(env);

    for (int j = 0; j < manager.components.size(); ++j) {
      const auto &component = manager.components[j];

      // Create column: obj coef 1.0 + constraints coef 1.0
      IloNumColumn col = obj(1.0);
      for (int idx : component.pontos_indices) {
        col += constraints[idx](1.0);
      }

      IloNumVar var(col, 0.0, 1.0, ILOINT);
      x.add(var);
      col.end();

      if (warm_start) {
        mipVar.add(var);
        mipVal.add(component.eh_sol ? 1.0 : 0.0);
      }
    }

    IloCplex cpl(model);
    cpl.setParam(IloCplex::Param::RandomSeed, seed);

    int aux_bsf = (int)bsf;
    if (cplex_abort) {
      IloCplex::Aborter abo(env);
      cpl.use(abo);
      cpl.use(abortCallback(env, aux_bsf));
    }

    if (warm_start) {
      cpl.addMIPStart(mipVar, mipVal, IloCplex::MIPStartAuto);
      mipVar.end();
      mipVal.end();
    }

    cpl.setParam(IloCplex::TiLim, cplex_time_limit);
    cpl.setParam(IloCplex::EpGap, 0.0);
    cpl.setParam(IloCplex::EpAGap, 0.0);
    cpl.setParam(IloCplex::Threads, 1);
    cpl.setParam(IloCplex::Param::Emphasis::MIP, heuristic_emphasis);

    cpl.setWarning(env.getNullStream());

    // Register callbacks
    // cpl.use(RandomizedRoundingCallback(env, x, 0.0));
    // cpl.use(GreedySetCoverCallback(env, x, 0.0));

    // calling CPLEX to solve the model
    cpl.solve();

    if (cpl.getStatus() == IloAlgorithm::Optimal ||
        cpl.getStatus() == IloAlgorithm::Feasible) {
      solution_cplex = cpl.getObjValue();
      IloNumArray sol(env);
      cpl.getValues(sol, x);

      if (solution_cplex > bsf) {
        for (auto &component : manager.components) {
          if (component.eh_sol) {
            component.idade = 0;
            component.vida++;
          } else {
            component.idade++;
            component.vida++;
          }
        }
      } else {
        for (int i = 0; i < manager.components.size(); ++i) {
          auto &component = manager.components[i];
          if (sol[i] > 0.5) {
            component.idade = 0;
            component.eh_sol = true;
            component.vida++;
          } else {
            component.idade++;
            component.eh_sol = false;
            component.vida++;
          }
        }
      }
      sol.end();
    }

  } catch (IloException &e) {
    cerr << "Concert exception caught: " << e << endl;
  }
  env.end();
  return solution_cplex;
}

// Assumindo que essas variáveis e a classe 'manager' estão definidas em algum
// lugar extern Manager manager; extern int n_pon; extern double bsf;

struct ComponentComparator {
  bool operator()(const Component &lhs, const Component &rhs) const {
    return lhs.pontos_indices.size() >
           rhs.pontos_indices.size(); // Ordena pelo número de pontos cobertos
                                      // em ordem decrescente
  }
};

double guloso() {
  // Ordena os componentes pelo número de pontos que cobrem em ordem decrescente
  std::sort(manager.components.begin(), manager.components.end(),
            ComponentComparator());

  std::vector<bool> visitado(n_pon, false);
  double sol = 0.0;

  // Percorre os componentes ordenados
  for (auto &component : manager.components) {
    bool fazParteSolucao = false;

    // Verifica se o componente cobre pontos não visitados
    for (int idx : component.pontos_indices) {
      if (!visitado[idx]) {
        fazParteSolucao = true;
        break;
      }
    }

    if (fazParteSolucao) {
      // Se o componente faz parte da solução, define sua idade como zero e
      // marca os pontos como visitados
      ++sol;
      component.idade = 0;
      for (int idx : component.pontos_indices) {
        visitado[idx] = true;
      }
    } else {
      // Se o componente não faz parte da solução, incrementa sua idade
      ++component.idade;
    }
  }

  if (bsf < sol) {
    for (auto &component : manager.components) {
      if (component.eh_sol)
        component.idade = 0;
      else
        ++component.idade;
    }
    return bsf;
  } else {
    for (auto &component : manager.components) {
      if (component.idade == 0)
        component.eh_sol = true;
      else
        component.eh_sol = false;
    }
    return sol;
  }
}

int greedySetCover() {
  // 1. Initialize data structures
  // Inverted index: point_idx -> list of component indices covering it
  std::vector<std::vector<int>> point_to_components(n_pon);
  // Current score of each component (number of uncovered points it covers)
  std::vector<int> component_score(manager.components.size(), 0);
  // Track covered points
  std::vector<bool> is_covered(n_pon, false);
  int uncovered_count = n_pon;

  // Priority queue to store {-score, component_index}
  // We use negative score because set sorts in ascending order, and we want max
  // score. Using set instead of priority_queue to allow efficient updates
  // (erase/insert).
  std::set<std::pair<int, int>> pq;

  // 2. Build inverted index and initial scores
  for (int i = 0; i < manager.components.size(); ++i) {
    const auto &component = manager.components[i];
    int score = 0;
    for (int idx : component.pontos_indices) {
      point_to_components[idx].push_back(i);
      score++;
    }
    component_score[i] = score;
    if (score > 0) {
      pq.insert({-score, i});
    }
  }

  std::vector<int> final_cover_indices;

  // 3. Main loop
  while (uncovered_count > 0 && !pq.empty()) {
    // Get best component
    auto best_it = pq.begin();
    int best_score = -best_it->first;
    int best_component_idx = best_it->second;
    pq.erase(best_it);

    // If best score is 0, we can't cover any more points
    if (best_score == 0)
      break;

    // Add to solution
    final_cover_indices.push_back(best_component_idx);

    // Update state for newly covered points
    const auto &best_component = manager.components[best_component_idx];
    for (int point_idx : best_component.pontos_indices) {
      if (is_covered[point_idx])
        continue;

      is_covered[point_idx] = true;
      uncovered_count--;

      // For each component that covers this now-covered point, decrease its
      // score
      for (int comp_idx : point_to_components[point_idx]) {
        if (comp_idx == best_component_idx)
          continue; // Skip the one we just picked

        // Only update if it's in the PQ (meaning it had score > 0)
        // We need to find it in PQ. Since we don't know the exact score in PQ
        // easily without looking it up, we use the stored component_score.
        int old_score = component_score[comp_idx];
        if (old_score > 0) {
          auto it = pq.find({-old_score, comp_idx});
          if (it != pq.end()) {
            pq.erase(it);
            int new_score = old_score - 1;
            component_score[comp_idx] = new_score;
            if (new_score > 0) {
              pq.insert({-new_score, comp_idx});
            }
          }
        }
      }
    }
  }

  if (uncovered_count > 0) {
    std::cerr << "Error: Cannot cover all elements in the universe. Uncovered: "
              << uncovered_count << std::endl;
  }

  // Update component states based on solution quality
  if (bsf > final_cover_indices.size()) {
    // Reset all components
    for (auto &component : manager.components) {
      component.eh_sol = false;
      component.idade = component.idade + 1;
      component.vida = component.vida + 1;
    }

    // Mark selected components as solution
    for (int idx : final_cover_indices) {
      manager.components[idx].eh_sol = true;
      manager.components[idx].idade = 0;
      manager.components[idx].vida = manager.components[idx].vida + 1;
    }

    return final_cover_indices.size();
  } else {
    // Keep current solution, just age components
    for (auto &component : manager.components) {
      if (component.eh_sol) {
        component.idade = 0;
      } else {
        component.idade = component.idade + 1;
        component.vida = component.vida + 1;
      }
    }

    return bsf;
  }
}

#endif // Cplex_H
