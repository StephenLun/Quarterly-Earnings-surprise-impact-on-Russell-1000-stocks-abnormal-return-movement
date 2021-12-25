//
//  Bootstrap.hpp
//  FRE6883_FinalProject
//
//  Created by Team_5 on 2021/12/11.
//

#pragma once

#include <stdio.h>
#include <vector>
#include <string>
#include "Stock.hpp"

using namespace std;

typedef vector<double> Vector;
typedef vector<vector<string>> Group;
typedef vector<vector<double>> Matrix;
typedef map<string, vector<double>> AbnormalReturns;
typedef map<string, vector<double>> Returns;
//typedef map<string, Stock*> Stocks;


// Operators of Vectors and Matrix
Vector operator+(Vector& V, Vector& W);
Vector operator+(vector<vector<double>>& M, Vector& V);
Vector operator-(Vector& V, Vector& W);
Matrix operator-(Matrix& M, Vector& V);
Vector operator-(Vector& V, double num);
Vector operator/(Vector& V, int num);
Vector SumSquare(Matrix& M);
Vector SquareRoot(Vector& V);

Group DivideGroup(map<string, Stock*> &stocks);

// Implement Bootstrapping
class Bootstrap
{
private:
    int SampleNum = 80;
    int N = 60;
    int RepeatNum = 40;

public:
    Group RandomSelect(Group grouplist); // randomly select 80 stocks from each group, return group vector
    void SetN(int N_);
    int GetN() { return N; };
    vector<AbnormalReturns> RetrieveRandomList(map<string, Stock*>& stocks, const Group& grouplist); // retrieve AR for 2N days
    Vector CalculateDailyAAR(AbnormalReturns& AbnormalReturn);
    Vector CalculateDailyCAAR(vector<double>& DailyAAR);
    vector<Matrix> finalCalculation(map<string, Stock*>& stocks, Group& grouplist);
};

