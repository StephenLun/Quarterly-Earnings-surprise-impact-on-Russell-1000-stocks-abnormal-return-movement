//
//  Stock.cpp
//  FRE6883_FinalProject
//
//  Created by Team_5 on 2021/12/10.
//

#include "Stock.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "Bootstrap.hpp"
using namespace std;

void Stock::CalculateRet() // calculate returns
{
    for (int t = 0; t < (int)prices.size() - 1; t++)
    {
        double Rit = (prices[t + 1] - prices[t]) / prices[t];
        returns.push_back(Rit);
    }
}

void Stock::CalculateAR()
{
    vector<double> benchmark_returns;
    for (int t = 0; t < (int)benchmark_prices.size() - 1; t++)
    {
        double Rit = (benchmark_prices[t + 1] - benchmark_prices[t]) / benchmark_prices[t];
        benchmark_returns.push_back(Rit);
    }
    abnormalreturns = returns - benchmark_returns;
}


void Stock::display(int N)
{
    cout << "Ticker: " << ticker << endl;
    cout << "Group: " << group << endl;
    cout << "Announce Date: " << date << endl;
    cout << "Period End: " << period_end << endl;
    cout << "Estimate EPS: " << estimate_eps << endl;
    cout << "Reported EPS: " << reported_eps << endl;
    cout << "Surprise: " << surprise << endl;
    cout << "Surprise Percent: " << surprise_percent << endl;
    cout << "Daily Prices and Cumulative Daily Returns:" << endl << endl;
    cout << setw(15) << "Dates"
         << setw(15) << "Prices"
         << setw(15) << "Cum Returns"
         << endl << endl;
    
    double cumreturns = 0.0;
    int startindex = day0index - N;
    for (int i = day0index - N; i <= day0index + N; i++) {
        cumreturns = prices[i] / prices[startindex] - 1;
        cout << setw(15) << dates[i]
             << setw(15) << prices[i]
             << setw(15) << cumreturns
             << endl;
        cout << endl;
    }
    cout << endl;
}

