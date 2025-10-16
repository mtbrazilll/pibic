#ifndef Cplex_H
#define Cplex_H

#pragma once
#include <ilcplex/ilocplex.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <limits>

#include "Componentes.hpp"
#include <stack>
#include <unordered_set>
#include <functional>
#include <unordered_map>
#include "Struct.h"

using std::vector;
using std::string;
using std::stack;
using std::unordered_set;
using std::function;

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

ILOSTLBEGIN

ILOMIPINFOCALLBACK2(Callback,
                    IloBool,  aborted,
                    IloNum,   sol)
{
       
    if ( !aborted  &&  hasIncumbent() ) {

        IloNum cost = getIncumbentObjValue();
        
        //printf("Found solution value: %f\n", cost);

        if ( cost < sol ) {
            //printf("Cost is better than bsf and time limit run out. Quiting after %fs...\n", timeUsed);
            aborted = IloTrue;
            abort();
        }
    }
}

ILOSOLVECALLBACK2(abortCallback, IloCplex::Aborter&, abo, int&, curbest) {
    if (hasIncumbent()) {
        IloNum nv = getIncumbentObjValue();
        if (curbest > int(nv)) abo.abort();
    }
}

ILOSIMPLEXCALLBACK0(MyCallback) {
  std::cout << "Iteration " << getNiterations() << ": ";
  if ( isFeasible() ) {
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

    try 
    {
        IloModel model(env);
        IloIntVarArray x(env, manager.getComponentCount(), 0, 1);

        // preparing warm-start
        IloNumVarArray mipVar(env);
        IloNumArray mipVal(env);
        if (warm_start) {
            int indice_var = 0;
            for (auto& component : manager.components) 
                {
    
                // Adicione cada variável à lista de variáveis de início
                    mipVar.add(x[indice_var]);

                // Adicione o valor inicial correspondente à lista de valores de início
                
                    mipVal.add(component.eh_sol ? 1 : 0);   
                    indice_var++;
                }
            
        }
             // end preparing warm-start

        IloExpr obj(env);
        IloExprArray componente_point(env, n_pon);

        int indice_var = 0;
        for (const auto& component : manager.components) 
        {
            obj += x[indice_var];

            for (const auto& ponto : component.points) {

                if (componente_point[ponto.indice].getImpl() == 0) {
                    componente_point[ponto.indice] = IloExpr(env);
                }
                componente_point[ponto.indice] += x[indice_var];
            }

            indice_var++;
        }
        for (int i = 0; i < n_pon; ++i) {
            if (!componente_point[i].getImpl() == 0) {
                model.add(componente_point[i] >= 1);
                componente_point[i].end();
            }
        }
        if (loops !=0){

            //model.add(obj >= bsf*0.8);
            //model.add(obj <= bsf);
        }
        model.add(IloMinimize(env, obj));
        obj.end();

        IloCplex cpl(model);
        /* the aborter stops CPLEX once a better solution than
        "best_sol" is found */

        int aux_bsf = bsf;
        if (cplex_abort) {
            IloCplex::Aborter abo(env);
            cpl.use(abo);
            cpl.use(abortCallback(env, abo, aux_bsf));
        }
        if (warm_start) cpl.addMIPStart(mipVar, mipVal);

        cpl.setParam(IloCplex::TiLim, cplex_time_limit);
        cpl.setParam(IloCplex::EpGap, 0.0);
        cpl.setParam(IloCplex::EpAGap, 0.0);
        cpl.setParam(IloCplex::Threads, 1);
        //if (heuristic_emphasis) 
        cpl.setParam(IloCplex::Param::Emphasis::MIP, heuristic_emphasis);
        cpl.setWarning(env.getNullStream());

        // calling CPLEX to solve the model
        cpl.solve();
        
        if (cpl.getStatus() == IloAlgorithm::Optimal or cpl.getStatus() == IloAlgorithm::Feasible)
        {
            IloNumArray sol(env, manager.getComponentCount());
            solution_cplex = cpl.getObjValue();

            //std::cout << solution_cplex << std::endl;
            if (solution_cplex > bsf){
                 
                 for (auto& component : manager.components){
                   
                    if (component.eh_sol) {
                        component.idade = 0;
                        component.eh_sol = true;
                    }
                    else {
                        component.idade = component.idade + 1;
                        component.eh_sol = false;
                    }
                 }

            }
            else {
                cpl.getValues(sol, x);
                indice_var = 0;
                for (auto& component : manager.components) {
                    
                    if(sol[indice_var] > 0.8){
                        component.idade = 0;
                        component.eh_sol = true;

                    }
                    else{

                        component.idade = component.idade + 1;
                        component.eh_sol = false;

                    }
                    indice_var++;
                }

            }
            
           

        }   
    }
       
    catch (IloException& e) {
        cerr << "Concert exception caught: " << e << endl;
    }
    env.end();
    return solution_cplex;
}




// Assumindo que essas variáveis e a classe 'manager' estão definidas em algum lugar
// extern Manager manager; 
// extern int n_pon;
// extern double bsf;

double cplex_colunas_geracao() {
    double solution_cplex = std::numeric_limits<double>::max();
    IloEnv env;
    env.setOut(env.getNullStream());
    env.setWarning(env.getNullStream());
    std::vector<int> rmp_var_to_component_idx;
    // Para rastrear quais componentes já estão no RMP
        std::vector<bool> componente_no_rmp(manager.components.size(), false);
    try {
        // --- ETAPA 1: CONFIGURAR O PROBLEMA MESTRE RESTRITO (RMP) ---
        IloModel rmpModel(env);

        // O RMP terá apenas as restrições, mas sem variáveis (colunas) inicialmente.
        IloRangeArray constraints(env, n_pon);
        for (int i = 0; i < n_pon; ++i) {
            // Cada ponto 'i' precisa ser coberto: sum(x_j) >= 1
            constraints[i] = IloRange(env, 1.0, IloInfinity);
        }
        rmpModel.add(constraints);

        // A função objetivo começa vazia.
        IloObjective objective = IloMinimize(env);
        rmpModel.add(objective);
        
        // Array para manter as variáveis do RMP. Começa vazio!
        IloNumVarArray x_rmp(env);

        IloNumVarArray artificial_vars(env);
        const double BIG_M = 1; // Um custo alto o suficiente

        for (int j = 0; j < manager.components.size(); ++j) {
    
            // Verifica se este componente faz parte da solução inicial
            if (manager.components[j].eh_sol) {
                
                const auto& initial_component = manager.components[j];

                // É crucial manter o rastreamento, assim como você faz no loop principal
                rmp_var_to_component_idx.push_back(j);
                componente_no_rmp[j] = true; // Marca que este componente já está no RMP

                // Cria a nova coluna (variável) para este componente
                // 1. Custo 1.0 na função objetivo
                IloNumColumn newCol = objective(1.0); 
                
                // 2. Coeficiente 1.0 em cada restrição de ponto que ele cobre
                for (const auto& ponto : initial_component.points) {
                    newCol += constraints[ponto.indice](1.0);
                }

                // 3. Adiciona a variável (coluna) ao array de variáveis do RMP
                x_rmp.add(IloNumVar(newCol, 0.0, IloInfinity));
                
                // 4. Libera a memória da coluna temporária
                newCol.end();
            }
        }

        

        // --- ETAPA 2: O LOOP PRINCIPAL DE GERAÇÃO DE COLUNAS ---
        int auxconta = -500;
        while (true) {
            auxconta++;

            if (auxconta == 20){
                break;
            }
            // --- ETAPA 2.1: RESOLVER O RMP ATUAL ---
            IloCplex cplex_rmp(rmpModel);
            cplex_rmp.setParam(IloCplex::Param::RootAlgorithm, IloCplex::Primal); // Primal Simplex é bom para RMPs
            cplex_rmp.setParam(IloCplex::Threads, 1);
            cplex_rmp.solve();

            if (cplex_rmp.getStatus() != IloAlgorithm::Optimal) {
                cerr << "Erro: RMP não pôde ser resolvido para o ótimo." << endl;
                break;
            }

            // --- ETAPA 2.2: OBTER OS PREÇOS DUAIS ---
            IloNumArray duals(env, n_pon);
            cplex_rmp.getDuals(duals, constraints);

            // --- ETAPA 2.3: RESOLVER O SUBPROBLEMA (PRICING) ---
            // O subproblema é encontrar o componente com o custo reduzido mais negativo.
            
            double minReducedCost = 0.0;
            int bestComponentIndex = -1;

            for (int j = manager.components.size() - 1; j >= 0; --j) {
                // Só consideramos componentes que AINDA NÃO estão no RMP
                if (componente_no_rmp[j]) {
                    continue;
                }

                const auto& component = manager.components[j];
                
                // Custo do componente (no seu código original, o custo era 1 para todos)
                double componentCost = 1.0; 

                // Calcular o "valor" do componente com base nos duais
                double dualValue = 0.0;
                //std::cout << "inicio" << std::endl;
                for (const auto& ponto : component.points) {
                    dualValue += duals[ponto.indice];
                    //std::cout << "valor dual ponto " << ponto.indice << " " << duals[ponto.indice] << std::endl;
                }
                //std::cout << "componente " << j << " valor " << dualValue << std::endl;
                //std::cout << "componente " << j << " pontos " << component.points.size() << std::endl;
                // Custo Reduzido = Custo - Valor
                double reducedCost = componentCost - dualValue;

                if (reducedCost < minReducedCost) {
                    minReducedCost = reducedCost;
                    bestComponentIndex = j;
                }
            }

            // --- ETAPA 2.4: DECIDIR SE ADICIONA UMA COLUNA ---
            // Usamos uma pequena tolerância para comparações de ponto flutuante
            const double epsilon = 1e-6;

            if (minReducedCost < -epsilon) {
                // SUCESSO! Encontramos uma coluna promissora. Vamos adicioná-la ao RMP.
                const auto& bestComponent = manager.components[bestComponentIndex];
                rmp_var_to_component_idx.push_back(bestComponentIndex);
                // Criar a nova coluna (variável)
                IloNumColumn newCol = objective(1.0); // Custo 1 na função objetivo
                for (const auto& ponto : bestComponent.points) {
                    newCol += constraints[ponto.indice](1.0); // Coeficiente 1 nas restrições que cobre
                }

                // Adicionar a nova variável ao RMP
                x_rmp.add(IloNumVar(newCol, 0.0, IloInfinity));
                newCol.end();
                
                componente_no_rmp[bestComponentIndex] = true; // Marcar como adicionado

            } else {
                // FIM! Nenhuma coluna melhora a solução. Ótimo do LP encontrado.
                break; 
            }
            cplex_rmp.end();
        }

        
        // Agora que temos todas as colunas boas, resolvemos o problema como um MIP.
        rmpModel.add(IloConversion(env, x_rmp, ILOBOOL)); // Converte todas as vars para inteiras
        
        IloCplex cplex_final(rmpModel);
        cplex_final.setParam(IloCplex::EpGap, 0.0);
        cplex_final.setParam(IloCplex::Threads, 1);
        cplex_final.solve();
        
        if (cplex_final.getStatus() == IloAlgorithm::Optimal || cplex_final.getStatus() == IloAlgorithm::Feasible) {
            solution_cplex = cplex_final.getObjValue();
            
            // Aqui você colocaria sua lógica para processar a solução final,
            // similar ao que você já tinha, mas iterando sobre as variáveis em x_rmp
            // e mapeando-as de volta para os componentes originais.
                     // ========================================================================
            // ### INÍCIO DO CÓDIGO DE ATUALIZAÇÃO DOS COMPONENTES ###

            if (solution_cplex > bsf){ 
                 // Apenas incrementa a idade dos componentes que não estão na solução anterior
                 for (auto& component : manager.components){
                    if (!component.eh_sol) {
                        component.idade++;
                    }
                 }
            }
            else { // A nova solução é melhor, então atualizamos com base nela
                
                IloNumArray sol_values(env, x_rmp.getSize());
                cplex_final.getValues(sol_values, x_rmp);

                // 1. Primeiro, reseta o status de todos os componentes
                for (auto& component : manager.components) {
                    component.eh_sol = false;
                }

                // 2. Marca apenas os componentes que estão na nova solução ótima
                for (IloInt i = 0; i < x_rmp.getSize(); ++i) {
                    // Usar uma tolerância para comparação de ponto flutuante
                    if (sol_values[i] > 0.8) { 
                        int original_component_idx = rmp_var_to_component_idx[i];
                        manager.components[original_component_idx].eh_sol = true;
                    }
                }

                // 3. Agora, atualiza a idade de todos com base no status final
                for (auto& component : manager.components) {
                    if (component.eh_sol) {
                        component.idade = 0; // Faz parte da solução, zera a idade
                    } else {
                        component.idade++; // Não faz parte, envelhece
                    }
                }
                sol_values.end();
            }
            // ========================================================================
            // ### FIM DO CÓDIGO DE ATUALIZAÇÃO ###
            // ========================================================================

        }

    } catch (IloException& e) {
        cerr << "Concert exception caught: " << e << endl;
    }
    env.end();
    return solution_cplex;
}

struct ComponentComparator {
    bool operator()(const Component& lhs, const Component& rhs) const {
        return lhs.points.size() > rhs.points.size(); // Ordena pelo número de pontos cobertos em ordem decrescente
    }
};

std::vector<bool> visitado;

double guloso() {
    // Ordena os componentes pelo número de pontos que cobrem em ordem decrescente
    std::sort(manager.components.begin(), manager.components.end(), ComponentComparator());

     

    // Inicializa o vetor visitado
    visitado.assign(n_pon, false);
    double sol = 0.0;
    // Percorre os componentes ordenados
    for (auto& component : manager.components) {
        bool fazParteSolucao = false;

        // Verifica se o componente cobre pontos não visitados
        for (const auto& ponto : component.points) {
            //std::cout << ponto.indice << "cima \n";
            if (!visitado[ponto.indice]) {
                fazParteSolucao = true;
                break;
            }
        }

        if (fazParteSolucao) {
            // Se o componente faz parte da solução, define sua idade como zero e marca os pontos como visitados
            ++sol;
            component.idade = 0;
            for (const auto& ponto : component.points) {
                //std::cout << ponto.indice << "baixo\n";
                visitado[ponto.indice] = true;
            }
        } else {
            // Se o componente não faz parte da solução, incrementa sua idade
            ++component.idade;
        }
    }
    if (bsf < sol){ // nunca acontece na primeira iteracao 
        for (auto& component : manager.components){
            if(component.eh_sol) component.idade = 0;
            else ++component.idade;
        }
        return bsf;

    }
    else{
        for (auto& component : manager.components){
            if(component.idade == 0) component.eh_sol = true;
            else component.eh_sol = false;
        }
        return sol;
    }
    //std::cout<<sol<<std::endl;
    
}

int greedySetCover() 
{
    // 1. Initialize uncovered elements set
    std::unordered_set<int> uncovered_elements;
    for (int i = 0; i < n_pon; ++i) {
        uncovered_elements.insert(i);
    }
    
    std::vector<int> final_cover_indices;
    int sol = 0;

    // 2. Main loop: continue until all elements are covered
    while (!uncovered_elements.empty()) {
        int best_set_index = -1;
        int max_covered_count = 0;

        // 3. Find the best component in current iteration
        for (int i = 0; i < manager.components.size(); ++i) {
            int current_covered_count = 0;
            // Check how many uncovered elements this component covers
            for (const auto& point : manager.components[i].points) {
                if (uncovered_elements.count(point.indice)) {
                    current_covered_count++;
                }
            }

            // If this component covers more new elements than the best found so far
            if (current_covered_count > max_covered_count) {
                max_covered_count = current_covered_count;
                best_set_index = i;
            }
        }

        // If no component can cover new elements, something is wrong
        if (best_set_index == -1 || max_covered_count == 0) {
            if (!uncovered_elements.empty()) {
                std::cerr << "Error: Cannot cover all elements in the universe." << std::endl;
            }
            break; 
        }
        
        // Add the best component to our final solution
        final_cover_indices.push_back(best_set_index);
        sol++;

        // Remove newly covered elements from uncovered set
        for (const auto& point : manager.components[best_set_index].points) {
            uncovered_elements.erase(point.indice);
        }
    }

    // Update component states based on solution quality
    if (bsf > final_cover_indices.size()) {
        // Reset all components
        for (auto& component : manager.components) {
            component.eh_sol = false;
            component.idade = component.idade + 1;
        }

        // Mark selected components as solution
        for (int idx : final_cover_indices) {
            manager.components[idx].eh_sol = true;
            manager.components[idx].idade = 0;
        }
        
        return final_cover_indices.size();
    } else {
        // Keep current solution, just age components
        for (auto& component : manager.components) {
            if (component.eh_sol) {
                component.idade = 0;
            } else {
                component.idade = component.idade + 1;
            }
        }
        
        return bsf;
    }
}




double cplex_colunas_geracao_mt_lento() {
    double solution_cplex = std::numeric_limits<double>::max();
    IloEnv env;
    env.setOut(env.getNullStream());
    env.setWarning(env.getNullStream());
    std::vector<int> rmp_var_to_component_idx;
    try {
        // --- ETAPA 1: CONFIGURAR O PROBLEMA MESTRE RESTRITO (RMP) ---
        IloModel rmpModel(env);

        // O RMP terá apenas as restrições, mas sem variáveis (colunas) inicialmente.
        IloRangeArray constraints(env, n_pon);
        for (int i = 0; i < n_pon; ++i) {
            // Cada ponto 'i' precisa ser coberto: sum(x_j) >= 1
            constraints[i] = IloRange(env, 1.0, IloInfinity);
        }
        rmpModel.add(constraints);

        // A função objetivo começa vazia.
        IloObjective objective = IloMinimize(env);
        rmpModel.add(objective);
        
        // Array para manter as variáveis do RMP. Começa vazio!
        IloNumVarArray x_rmp(env);

        IloNumVarArray artificial_vars(env);
        const double BIG_M = 10000.0; // Um custo alto o suficiente

        for (int i = 0; i < n_pon; ++i) {
            // Cria uma variável artificial para a restrição 'i'
            // Nome: art_i, Limites: [0, Infinito], Custo: BIG_M, Coeficiente 1 na restrição 'i'
            IloNumColumn col = objective(BIG_M) + constraints[i](1.0);
            artificial_vars.add(IloNumVar(col, 0.0, IloInfinity));
            col.end();
        }

        // Para rastrear quais componentes já estão no RMP
        std::vector<bool> componente_no_rmp(manager.components.size(), false);

        // --- ETAPA 2: O LOOP PRINCIPAL DE GERAÇÃO DE COLUNAS ---
        while (true) {

            // --- ETAPA 2.1: RESOLVER O RMP ATUAL ---
            IloCplex cplex_rmp(rmpModel);
            //cplex_rmp.setParam(IloCplex::Param::RootAlgorithm, IloCplex::Primal); // Primal Simplex é bom para RMPs
            cplex_rmp.setParam(IloCplex::Threads, 1);
            cplex_rmp.solve();

            if (cplex_rmp.getStatus() != IloAlgorithm::Optimal) {
                cerr << "Erro: RMP não pôde ser resolvido para o ótimo." << endl;
                break;
            }

            // --- ETAPA 2.2: OBTER OS PREÇOS DUAIS ---
            IloNumArray duals(env, n_pon);
            cplex_rmp.getDuals(duals, constraints);

            // --- ETAPA 2.3: RESOLVER O SUBPROBLEMA (PRICING) ---
            // O subproblema é encontrar o componente com o custo reduzido mais negativo.
            
            double minReducedCost = 0.0;
            int bestComponentIndex = -1;

            for (int j = 0; j < manager.components.size(); ++j) {
                // Só consideramos componentes que AINDA NÃO estão no RMP
                if (componente_no_rmp[j]) {
                    continue;
                }

                const auto& component = manager.components[j];
                
                // Custo do componente (no seu código original, o custo era 1 para todos)
                double componentCost = 1.0; 

                // Calcular o "valor" do componente com base nos duais
                double dualValue = 0.0;
                std::cout << "inicio" << std::endl;
                for (const auto& ponto : component.points) {
                    dualValue += duals[ponto.indice];
                    std::cout << "valor dual ponto " << ponto.indice << " " << duals[ponto.indice] << std::endl;
                }

                // Custo Reduzido = Custo - Valor
                double reducedCost = componentCost - dualValue;

                if (reducedCost < minReducedCost) {
                    minReducedCost = reducedCost;
                    bestComponentIndex = j;
                }
            }

            // --- ETAPA 2.4: DECIDIR SE ADICIONA UMA COLUNA ---
            // Usamos uma pequena tolerância para comparações de ponto flutuante
            const double epsilon = 1e-6;

            if (minReducedCost < -epsilon) {
                // SUCESSO! Encontramos uma coluna promissora. Vamos adicioná-la ao RMP.
                const auto& bestComponent = manager.components[bestComponentIndex];
                rmp_var_to_component_idx.push_back(bestComponentIndex);
                // Criar a nova coluna (variável)
                IloNumColumn newCol = objective(1.0); // Custo 1 na função objetivo
                for (const auto& ponto : bestComponent.points) {
                    newCol += constraints[ponto.indice](1.0); // Coeficiente 1 nas restrições que cobre
                }

                // Adicionar a nova variável ao RMP
                x_rmp.add(IloNumVar(newCol, 0.0, IloInfinity));
                newCol.end();
                
                componente_no_rmp[bestComponentIndex] = true; // Marcar como adicionado

            } else {
                // FIM! Nenhuma coluna melhora a solução. Ótimo do LP encontrado.
                break; 
            }
            cplex_rmp.end();
        }

        // --- ETAPA 3: RESOLVER O PROBLEMA FINAL COMO INTEIRO ---
        for (int i = 0; i < artificial_vars.getSize(); ++i) {
            artificial_vars[i].setUB(0.0);
        }
        // Agora que temos todas as colunas boas, resolvemos o problema como um MIP.
        rmpModel.add(IloConversion(env, x_rmp, ILOINT)); // Converte todas as vars para inteiras
        
        IloCplex cplex_final(rmpModel);
        cplex_final.setParam(IloCplex::EpGap, 0.0);
        cplex_final.setParam(IloCplex::Threads, 1);
        cplex_final.solve();
        
        if (cplex_final.getStatus() == IloAlgorithm::Optimal || cplex_final.getStatus() == IloAlgorithm::Feasible) {
            solution_cplex = cplex_final.getObjValue();
            
            // Aqui você colocaria sua lógica para processar a solução final,
            // similar ao que você já tinha, mas iterando sobre as variáveis em x_rmp
            // e mapeando-as de volta para os componentes originais.
                     // ========================================================================
            // ### INÍCIO DO CÓDIGO DE ATUALIZAÇÃO DOS COMPONENTES ###

            if (solution_cplex > bsf){ 
                 // Apenas incrementa a idade dos componentes que não estão na solução anterior
                 for (auto& component : manager.components){
                    if (!component.eh_sol) {
                        component.idade++;
                    }
                 }
            }
            else { // A nova solução é melhor, então atualizamos com base nela
                
                IloNumArray sol_values(env, x_rmp.getSize());
                cplex_final.getValues(sol_values, x_rmp);

                // 1. Primeiro, reseta o status de todos os componentes
                for (auto& component : manager.components) {
                    component.eh_sol = false;
                }

                // 2. Marca apenas os componentes que estão na nova solução ótima
                for (IloInt i = 0; i < x_rmp.getSize(); ++i) {
                    // Usar uma tolerância para comparação de ponto flutuante
                    if (sol_values[i] > 0.99) { 
                        int original_component_idx = rmp_var_to_component_idx[i];
                        manager.components[original_component_idx].eh_sol = true;
                    }
                }

                // 3. Agora, atualiza a idade de todos com base no status final
                for (auto& component : manager.components) {
                    if (component.eh_sol) {
                        component.idade = 0; // Faz parte da solução, zera a idade
                    } else {
                        component.idade++; // Não faz parte, envelhece
                    }
                }
                sol_values.end();
            }
            // ========================================================================
            // ### FIM DO CÓDIGO DE ATUALIZAÇÃO ###
            // ========================================================================

        }

    } catch (IloException& e) {
        cerr << "Concert exception caught: " << e << endl;
    }
    env.end();
    return solution_cplex;
}




#endif // Cplex_H
