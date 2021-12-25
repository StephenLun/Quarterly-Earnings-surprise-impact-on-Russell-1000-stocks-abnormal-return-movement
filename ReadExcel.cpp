//
//  ReadExcel.cpp
//  FRE6883_FinalProject
//
//  Created by Team_5 on 2021/12/10.
//

#include "ReadExcel.hpp"

map<string,Stock*> ReadExcel(string filename)
{
    ifstream ifs(filename);

    if (!ifs.is_open()) {
        cerr << "Open EarningsAnnouncements.csv Failed" << endl;
        system("pause");
    }

    string line;
    getline(ifs, line); //ignore first line

    map<string, Stock*> stock_list;

    while (getline(ifs, line))
    {
        Stock* stk = new Stock();

        istringstream ss_line(line);
        string item = "";

        getline(ss_line, item, ',');
        string ticker = item;
        stk->SetTicker(ticker);

        getline(ss_line, item, ',');
        string date = item;
        stk->SetDate(date);

        getline(ss_line, item, ',');
        string period_end = item;
        stk->SetPeriodEnd(period_end);

        getline(ss_line, item, ',');
        double estimate_eps = stod(item);
        stk->SetEstimateEPS(estimate_eps);

        getline(ss_line, item, ',');
        double reported_eps = stod(item);
        stk->SetReportedEPS(reported_eps);

        getline(ss_line, item, ',');
        double surprise = stod(item);
        stk->SetSurprise(surprise);

        getline(ss_line, item, ',');
        double surprise_percent = stod(item);
        stk->SetSurprisePercent(surprise_percent);

        getline(ss_line, item, ',');
        int group = stoi(item); // transform string to interger
        stk->SetGroup(group);

        stock_list[ticker] = stk;
        
        cout << ticker << " information loaded" << endl;

    }
    return stock_list;
}
