//
//  Bootstrap.cpp
//  FRE6883_FinalProject
//
//  Created by Team_5 on 2021/12/11.
//

#include "Bootstrap.hpp"
#include "stdio.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <iterator>
#include <cmath>
#include <chrono> // std::chrono::system_clock


// divide_group beat/meet/miss, create group vector [[],[],[]]
Group DivideGroup(map<string, Stock*> &stocks)
{
    Group grouplist(3);
    map<string, Stock*>::iterator itr;
    for (itr = stocks.begin(); itr != stocks.end(); itr++)
    {
        if (itr->second->GetGroup() == 1)
            grouplist[0].push_back(itr->second->GetTicker());
        else if (itr->second->GetGroup() == 2)
            grouplist[1].push_back(itr->second->GetTicker());
        else
            grouplist[2].push_back(itr->second->GetTicker());
    }
    return grouplist;
}



// Operators of Vectors and Matrix
// V, W are vectors of floating numbers with type vector<double>.
// M, N are matrices of floating numbers with type vector<vector<double>>.

// 1. Addition of 2 vectors
Vector operator+(Vector& V, Vector& W)
{
    int d = (int)V.size();
    Vector U(d);
    for (int j = 0; j < d; j++)
    {
        U[j] = V[j] + W[j];
    }
    return U;
}

// 2. Addition between matrix and vector
Vector operator+(vector<vector<double>>& M, Vector& V)
{
    int c = (int)M.size();
    int d = (int)V.size();
    Vector W(d);
    for (int j = 0; j < d; j++)
    {
        for (int i = 0; i < c; i++)
        {
            W[j] = M[i][j] + V[j];
        }
    }
    return W;
}

// 3. Subtraction between vector and vector
Vector operator-(Vector& V, Vector& W)
{
    int d = (int)V.size();
    Vector U(d);
    for (int j = 0; j < d; j++)
    {
        U[j] = V[j] - W[j];
    }
    return U;
}

// 4. Subtraction between matrix and vector
Matrix operator-(Matrix& M, Vector& V)
{
    int c = (int)M.size();
    int d = (int)V.size();
    Matrix W(c, Vector(d, 0.0));
    for (int j = 0; j < c; j++)
    {
        for (int i = 0; i < d; i++)
        {
            W[j][i] = M[j][i] - V[i];
        }
    }
    return W;
}

// 5. Subtraction between vector and number
Vector operator-(Vector& V, double num)
{
    int d = (int)V.size();
    Vector W(d);
    for (int j = 0; j < d; j++)
        W[j] = V[j] - num;
    return W;
}

// 6. Division between vector and number
Vector operator/(Vector& V, int num)
{
    int d = (int)V.size();
    Vector W(d);
    for (int j = 0; j < d; j++)
        W[j] = V[j] / num;
    return W;
}

// 7. SumSquare of an matrix
Vector SumSquare(Matrix& M)
{
    int d = (int)M[0].size();
    Vector V(d, 0.0);
    for (int i = 0; i < (int)M.size(); i++)
    {
        for (int j = 0; j < d; j++)
        {
            V[j] = V[j] + pow(M[i][j], 2);
        }
    }
    return V;
}

// 8. Square root of a vector
Vector SquareRoot(Vector& V)
{
    int d = (int)V.size();
    Vector U(d);
    for (int j = 0; j < d; j++)
    {
        U[j] = sqrt(V[j]);
    }
    return U;
}


//---------------------------------------------------------------------------------------------------------

Group Bootstrap::RandomSelect(Group grouplist)
{
    Group randomlist(3);
    for (int i = 0; i < 3; i++)
    {
        //random_shuffle(grouplist[i].begin(), grouplist[i].end());
        unsigned seed = (int)std::chrono::system_clock::now().time_since_epoch().count();
        //get the current time microsecond, Count the time span between the epoch and the time_point.
        shuffle(grouplist[i].begin(), grouplist[i].end(), std::default_random_engine(seed)); //set the seed
        for (int j = 0; j < SampleNum; j++)
        {
            randomlist[i].push_back(grouplist[i][j]);
        }
    }
    return randomlist;
}

void Bootstrap::SetN(int N_)
{
    N = N_;
}

// retrieve abnormal data, vector of 3 matrix, each is 80 * 2N
vector<AbnormalReturns> Bootstrap::RetrieveRandomList(map<string, Stock*>& stocks, const Group& randomlist)
{
    vector<AbnormalReturns> ARmap(3);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < SampleNum; j++)
        {
            Vector abnormalreturns = stocks[randomlist[i][j]]->GetAR(); // extract abnormal return
            int day0index = stocks[randomlist[i][j]]->GetDay0Index() - 1; // extract day0index minus one cuz the return vector has one less elements than price
            std::vector<double> ab_slice(abnormalreturns.begin() + day0index - N + 1, abnormalreturns.begin() + day0index + N + 1); // slice the abnormal vector
            ARmap[i].insert({ randomlist[i][j], ab_slice }); // use randomlist for key, ab_slice for value insert into AbnormalReturns (map)
        }
    }
    return ARmap;
}


Vector Bootstrap::CalculateDailyAAR(AbnormalReturns& AbnormalReturn) // calculate the AAR for a group
{
    vector<double> DailyAAR(2 * N, 0.0);
    for (int t = 0; t < 2 * N; t++)
    {
        for (map<string, vector<double>> ::iterator itr = AbnormalReturn.begin(); itr != AbnormalReturn.end(); itr++)
        {
            DailyAAR[t] += itr->second[t]; // AR at time t for each stock in the group
        }
        DailyAAR[t] = DailyAAR[t] / SampleNum; // divided by number of samples
    }
    return DailyAAR;
}

Vector Bootstrap::CalculateDailyCAAR(vector<double>& DailyAAR)
{
    vector <double> DailyCAAR(2 * N, 0.0);
    DailyCAAR[0] = DailyAAR[0];
    for (int t = 0; t < 2 * N - 1; t++)
    {
        DailyCAAR[t + 1] = DailyCAAR[t] + DailyAAR[t + 1]; // calculate CAAR using AAR, just cumsum
    }
    return DailyCAAR;
}



//--------------------------------
vector<Matrix> Bootstrap::finalCalculation(map<string, Stock*>& stocks, Group& grouplist)
{
    Matrix sumAAR(3, Vector(2 * N, 0.0)); // store the summation of AAR
    Matrix sumCAAR(3, Vector(2 * N, 0.0)); // store the summation of CAAR
    vector<Matrix> wholeAAR(3); // store whole 40 times of AAR
    vector<Matrix> wholeCAAR(3); // store whole 40 times of CAAR

    // Initialization
    for (int i = 0; i < 3; i++)
    {
        for (int n = 0; n < RepeatNum; n++)
        {
            wholeAAR[i].push_back(Vector(2 * N, 0.0));
            wholeCAAR[i].push_back(Vector(2 * N, 0.0));
        }
    }

    // repeat for 40 times
    for (int n = 0; n < RepeatNum; n++)
    {
        Group randomlist = RandomSelect(grouplist); // random select stocks for each group

        vector<AbnormalReturns> ARmap(3);
        ARmap = RetrieveRandomList(stocks, randomlist); // extract abnormal return

        for (int i = 0; i < 3; i++)
        {
            Vector AAR = CalculateDailyAAR(ARmap[i]);
            sumAAR[i] = sumAAR[i] + AAR; // sum each AAR for each experiment
            Vector CAAR = CalculateDailyCAAR(AAR);
            sumCAAR[i] = sumCAAR[i] + CAAR; // sum each CAAR
            wholeAAR[i][n] = AAR; //store each AAR
            wholeCAAR[i][n] = CAAR; // store each CAAR
        }
    }

    //after 40 times simulation, we calculate Ave and S.D.
    Matrix AveAAR(3, Vector(2 * N, 0.0));
    Matrix AveCAAR(3, Vector(2 * N, 0.0));
    Matrix SDAAR(3, Vector(2 * N, 0.0));
    Matrix SDCAAR(3, Vector(2 * N, 0.0));

    for (int i = 0; i < 3; i++)
    {
        AveAAR[i] = sumAAR[i] / RepeatNum;
        AveCAAR[i] = sumCAAR[i] / RepeatNum;
    }

    for (int i = 0; i < 3; i++)
    {
        Matrix Res = wholeAAR[i] - AveAAR[i]; // Matrix - vector, minus the mean for each AAR in whole
        Vector ResSquareSum = SumSquare(Res); // sum each row the sumsquare of residual
        Vector ResSquareAve = ResSquareSum / (RepeatNum - 1); // divided by n - 1 we get variance
        SDAAR[i] = SquareRoot(ResSquareAve); // squareroot we get std

        Res = wholeCAAR[i] - AveCAAR[i]; // same process
        ResSquareSum = SumSquare(Res);
        ResSquareAve = ResSquareSum / (RepeatNum - 1);
        SDCAAR[i] = SquareRoot(ResSquareAve);
    }

    //build final result matrix
    vector<Matrix> FinalResult(3, Matrix(4, Vector(2 * N, 0.0)));
    //Put Beat's average AAR, standard deviation of AAR, average CAAR, and standard deviation of CAAR in final result matrix
    for (int i = 0; i < 3; i++)
    {
        FinalResult[i][0] = AveAAR[i];
        FinalResult[i][1] = SDAAR[i];
        FinalResult[i][2] = AveCAAR[i];
        FinalResult[i][3] = SDCAAR[i];
    }

    return FinalResult;
}

