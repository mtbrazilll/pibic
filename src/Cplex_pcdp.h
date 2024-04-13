//CPLEX
#ifndef Cplex_pcdp_H
#define Cplex_pcdp_H

#include <ilcplex/ilocplex.h>
#include "SmallestEnclosingCircle.hpp"

#include <fstream>
#include <vector>

typedef IloArray<IloNumVarArray> NumVar2D; 
extern std::vector<Ponto> pontos;
const double eps = 1e-14;
extern long double max_x;
extern long double maior_em_modulo;


double solve(int n_point) 
{
    IloEnv env;
    IloModel Model(env);
    double cost = 0.0;
    double bigM = maior_em_modulo*maior_em_modulo*3;
    IloNumVar obj(env, 0, IloInfinity, ILOFLOAT);
    NumVar2D p(env, n_point);

    int n_disk = n_point;
    IloNumVarArray z(env, n_disk, 0, 1, ILOBOOL);
    IloNumVarArray X(env, n_disk, -IloInfinity, IloInfinity, ILOFLOAT);
    IloNumVarArray Y(env, n_disk, -IloInfinity, IloInfinity, ILOFLOAT);


    // region Constraints
    for (int i = 0; i < n_point; i++){
        p[i] = IloNumVarArray(env, n_disk, 0, 1, ILOBOOL);
    }

    for(int i = 0; i < n_disk; ++i) {
        for(int j = 0; j < n_point; ++j) {
            IloExpr distancia(env);
            distancia = (IloPower(pontos[j].point.x() - X[i], 2) + IloPower(pontos[j].point.y()  - Y[i], 2));

            Model.add( distancia <= 1.0*p[j][i] + bigM*(1-p[j][i]) );
            distancia.end();
        }
        
    }

    for (int j = 0; j < n_point; j++)
    {
        Model.add(IloSum(p[j]) >= 1);

        for (int i = 0; i < n_disk; i++)
        {
            Model.add(p[j][i] <= z[i]);
        }
        
    }
    
        IloExpr exp(env);
        for (int i = 0; i < n_disk; i++)
        {
            exp += z[i]*1.0;
        }
        Model.add(obj == exp);
    // Função objetivo.
    
    Model.add(IloMinimize(env, obj));
    
    // restrição p podar mais rapido
    //Model.add(z < bsf);

    // Solving
    IloCplex cplex(Model);
    //cplex.setOut(env.getNullStream()); 
    cplex.setParam(IloCplex::Param::TimeLimit, 1800.0);
    if (cplex.solve()){

        cost = cplex.getObjValue();

    }

    cout << cplex.getCplexStatus() << endl;
    
    cplex.end();
    Model.end();
    env.end();
    std::cout << "cplex_otimo: "<< cost << std::endl;
    std::cout << "bigM: "<< bigM << std::endl;
    return cost;  

                
}


#endif // Cplex_H