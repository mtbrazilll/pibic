//CPLEX
#ifndef Cplex_pcdp_H
#define Cplex_pcdp_H

#include <ilcplex/ilocplex.h>
#include "SmallestEnclosingCircle.hpp"

#include <fstream>
#include <vector>

typedef IloArray<IloNumVarArray> NumVar2D; 





extern std::vector<Ponto> pontos;

extern long double max_x;


double solve(int n_point) 
{
    IloEnv env;
    IloModel Model(env);
    double cost = 0.0;

    ofstream outfile;
    IloNumVar obj(env, 0, IloInfinity, ILOFLOAT);
    // Define decision variable
    NumVar2D p(env, n_point);
    for (int j = 0; j < n_point; j++){
        p[j] = IloNumVarArray(env, n_point, 0, 1, ILOBOOL);
        for (int i = 0; i < n_point; ++i) {
            char varName[100];
            sprintf(varName, "p_%d_%d", j, i);
            p[j][i].setName(varName);
        }
    }

    IloNumVarArray z(env, n_point, 0, 1, ILOBOOL);
    for (int i = 0; i < n_point; ++i) {
        char varName[100];
        sprintf(varName, "z_%d", i);
        z[i].setName(varName);
    }

    IloNumVarArray X(env, n_point, -IloInfinity, IloInfinity, ILOFLOAT);
    IloNumVarArray Y(env, n_point, -IloInfinity, IloInfinity, ILOFLOAT);
    for (int i = 0; i < n_point; ++i) {
        char varNameX[100], varNameY[100];
        sprintf(varNameX, "X_%d", i);
        sprintf(varNameY, "Y_%d", i);
        X[i].setName(varNameX);
        Y[i].setName(varNameY);
    }

    // region Constraints
    
    for(int i = 0; i < n_point; ++i) {
        for(int j = 0; j < n_point; ++j) {
            IloExpr distancia(env);
            distancia = (IloPower(pontos[j].point.x() - X[i], 2) + IloPower(pontos[j].point.y()  - Y[i], 2));

            Model.add(distancia <= p[j][i] + max_x*max_x*(1-p[j][i]));
            distancia.end();
        }
        
    }

    for (int j = 0; j < n_point; j++)
    {
        Model.add(IloSum(p[j]) >= 1);

        for (int i = 0; i < n_point; i++)
        {
            Model.add(p[j][i] <= z[i]);
        }
        
    }
    
    IloExpr exp(env);
    for (int i = 0; i < n_point; i++)
    {
        exp += z[i];
    }
    Model.add(obj == exp);

    // Função objetivo.
    Model.add(IloMinimize(env, obj));
    
    // restrição p podar mais rapido
    //Model.add(z < bsf);

    // Solving
    IloCplex cplex(Model);
    ///cplex.setOut(env.getNullStream());
    //cplex.setParam(IloCplex::Param::TimeLimit, 0.5); // limite de tempo pra resolver
    //cplex.setParam(IloCplex::Param::TimeLimit, time_limit); // limite de tempo pra resolver
    //cplex.setParam(IloCplex::EpGap,  0.05); //set the minimum required gap
    //cout << cplex.getModel() << endl;
    char filename[] = "model.lp"; // or "model.sav" for a binary format
    cplex.exportModel(filename);
    
    

    if (cplex.solve()){

        // Crie um objeto ofstream para escrever no arquivo de saída
            
        
        outfile.open("output.txt", ios::app);
        // Escreva as informações no arquivo de saída
        outfile << "Instance name: " << "000001" << "\n";
        
        outfile << "Points and their locations (x, y):\n" ;

        cost = cplex.getObjValue();
        outfile << "Optimal value: " << cost << "\n";
        //cout << cplex.getValue(z) << endl ;
        //cout << cplex.getObjValue() << endl << endl;
        //cout << cplex.getCplexStatus() << endl;

        // Obtendo a solução
        IloNumArray sol(env, n_point);
        cplex.getValues(sol, z);
        vector<int> disks;

        for (int i = 0; i < n_point; i++)
        {
            if (sol[i] > 0.5){
                outfile << "Disco " << i << " Pos (" << cplex.getValue(X[i]) << ", " << cplex.getValue(Y[i]) << ")\n";
                
            }  
            
        }
    }

    outfile << std::endl << std::endl;
    outfile.close();     

    cplex.end();
    Model.end();
    env.end();
    std::cout << cost;
    return cost;  

                
}


#endif // Cplex_H