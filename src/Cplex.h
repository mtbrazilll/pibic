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

ILOSTLBEGIN

ILOMIPINFOCALLBACK3(timeLimitCallback,
                    IloCplex, cplex,
                    IloBool,  aborted,
                    IloNum,   timeStart)
{
       
    if ( !aborted  &&  hasIncumbent() ) {
        IloNum timeUsed = cplex.getCplexTime() - timeStart;
        IloNum cost = getIncumbentObjValue();
        
        //printf("Found solution value: %f\n", cost);

        if ( timeUsed >= 3.0 || cost < bsf ) {
            //printf("Cost is better than bsf and time limit run out. Quiting after %fs...\n", timeUsed);
            aborted = IloTrue;
            abort();
        }
    }
}



double Exato_h(vector<Ponto> const &points) {
    double valor_otimo = std::numeric_limits<double>::max();
    

    IloEnv env;

    int n_pon = points.size(); // num de pontos
    int n_tuplas = manager.getComponentCount();

    IloModel antena(env);
    IloCplex cplex(antena);

    IloIntVarArray x(env, n_tuplas, 0, 1);
    IloExpr obj(env);
    IloExprArray componente_point(env, n_pon);

    int indice_var = 0;
    for (const auto& pair : manager.components) {
       
        const Component& component = pair.second;

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

    //cplex.setParam(IloCplex::Param::TimeLimit, 0.01); // limite de tempo pra resolver

    cplex.setOut(env.getNullStream());
    cplex.use(timeLimitCallback(env, cplex, IloFalse, cplex.getCplexTime()));

    cplex.solve(); 
    valor_otimo = cplex.getObjValue();
    IloNumArray sol(env, n_tuplas);
    cplex.getValues(sol, x);

    indice_var = 0;
    for (auto& pair : manager.components) {
        
        Component& component = pair.second;
        component.idade = (sol[indice_var] > 0.5) ? 0 : component.idade + 1;

        indice_var++;
    }

    env.end();
    return valor_otimo;
}

#endif // Cplex_H
