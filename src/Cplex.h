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
extern vector<int> i_var;
extern std::unordered_map<Point, std::size_t> pointToIndex;
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



double Exato_h(vector<Point> &points) {
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
        i_var[indice_var] = pair.first;
        const Component& component = pair.second;

        obj += x[indice_var];

        for (const auto& po : component.points) {
            auto it = pointToIndex.find(po);
            int i_p = it->second;
            if (componente_point[i_p].getImpl() == 0) {
                componente_point[i_p] = IloExpr(env);
            }
            componente_point[i_p] += x[indice_var];
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

    for (int i = 0; i < n_tuplas; i++) {
        Component *c = manager.getComponent(i_var[i]);
        c->idade = (sol[i] > 0.5) ? 0 : c->idade + 1;
    }

    env.end();
    return valor_otimo;
}

#endif // Cplex_H
