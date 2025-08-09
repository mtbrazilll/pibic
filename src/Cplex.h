#ifndef Cplex_H
#define Cplex_H

#pragma once
#include <ilcplex/ilocplex.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

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
        //model.add(obj >= bsf*0.8);
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

#endif // Cplex_H
