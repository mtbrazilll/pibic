#ifndef Cplex_H
#define Cplex_H

#pragma once
#include <ilcplex/ilocplex.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "SmallestEnclosingCircle.hpp"
#include "Componentes.hpp"
#include <stack>
#include <unordered_set>
#include <functional>
#include <unordered_map>

using std::vector;
using std::string;
using std::stack;
using std::unordered_set;
using std::function;

extern ComponentManager manager;
extern double bsf;
extern unsigned long int n_pon;

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


ILOSIMPLEXCALLBACK0(MyCallback) {
  std::cout << "Iteration " << getNiterations() << ": ";
  if ( isFeasible() ) {
     std::cout << "Objective = " << getObjValue() << endl;
  } else {
     std::cout << "Infeasibility measure = " << getInfeasibility() << endl;
  }
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

double Exato_h() {
    double valor_otimo = std::numeric_limits<double>::max();
    

    IloEnv env;

 
    int n_tuplas = manager.getComponentCount();

    IloModel antena(env);
    IloCplex cplex(antena);

    IloIntVarArray x(env, n_tuplas, 0, 1);
    IloExpr obj(env);
    IloExprArray componente_point(env, n_pon);

    int indice_var = 0;
    for (const auto& component : manager.components) {
       
        

        obj += x[indice_var];

        for (const auto& po : component.points) {

            if (componente_point[po.indice].getImpl() == 0) {
                componente_point[po.indice] = IloExpr(env);
            }
            componente_point[po.indice] += x[indice_var];
        }

        indice_var++;
    }

    for (int i = 0; i < n_pon; ++i) {
        if (!componente_point[i].getImpl() == 0) {
            antena.add(componente_point[i] >= 1);
        }
    }

    antena.add(IloMinimize(env, obj));





    //cplex.setParam(IloCplex::Param::TimeLimit, 1.0); // limite de tempo pra resolver

    cplex.setOut(env.getNullStream());
    cplex.use(Callback(env, IloFalse, bsf));
    cplex.solve();
    
    valor_otimo = cplex.getObjValue();
    IloNumArray sol(env, n_tuplas);
    cplex.getValues(sol, x);
    

    indice_var = 0;
    for (auto& component : manager.components) {
        
      
        if(sol[indice_var] > 0.5){
            component.idade = 0;
            component.eh_sol = true;

        }
        else{

            component.idade = component.idade + 1;
            component.eh_sol = true;

        }
        component.idade_rastreio++;
        indice_var++;
    }

    env.end();
    return valor_otimo;
}

double Exato_start() {
    double valor_otimo = std::numeric_limits<double>::max();
    

    IloEnv env;

    
    int n_tuplas = manager.getComponentCount();

    IloModel antena(env);
    IloCplex cplex(antena);

    IloIntVarArray x(env, n_tuplas, 0, 1);
    IloExpr obj(env);
    IloExprArray componente_point(env, n_pon);

    int indice_var = 0;
    for (const auto& component : manager.components) {
       
        

        obj += x[indice_var];

        for (const auto& po : component.points) {

            if (componente_point[po.indice].getImpl() == 0) {
                componente_point[po.indice] = IloExpr(env);
            }
            componente_point[po.indice] += x[indice_var];
        }

        indice_var++;
    }

    for (int i = 0; i < n_pon; ++i) {
        if (!componente_point[i].getImpl() == 0) {
            antena.add(componente_point[i] >= 1);
        }
    }

    antena.add(IloMinimize(env, obj));
    antena.add(obj<=bsf);


// Crie uma solução viável inicial
    IloNumVarArray startVar(env);
    IloNumArray startVal(env);
    indice_var = 0;
    for (auto& component : manager.components) {

      
        
    // Adicione cada variável à lista de variáveis de início
        startVar.add(x[indice_var]);
        // Adicione o valor inicial correspondente à lista de valores de início
        startVal.add(component.eh_sol);  // seus valores iniciais são 0 ou        
        indice_var++;
    }

    


    //cplex.setParam(IloCplex::Param::TimeLimit, 1.0); // limite de tempo pra resolver

    cplex.setOut(env.getNullStream());
    cplex.use(Callback(env, IloFalse, bsf));
    //cplex.use(MyCallback(env));
    //cplex.setParam(IloCplex::Param::RootAlgorithm, IloCplex::Primal);
    cplex.addMIPStart(startVar, startVal);

    cplex.solve();
    
    valor_otimo = cplex.getObjValue();
    IloNumArray sol(env, n_tuplas);
    cplex.getValues(sol, x);
    

    indice_var = 0;
    for (Component& component : manager.components) {
        
       
        if(sol[indice_var] > 0.5){
            component.idade = 0;
            component.eh_sol = 1;

        }
        else{

            component.idade = component.idade + 1;
            component.eh_sol = 0;

        }
        component.idade_rastreio++;
        indice_var++;
    }

    env.end();
    return valor_otimo;
}
#endif // Cplex_H
