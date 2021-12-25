//
//  Stock.hpp
//  FRE6883_FinalProject
//
//  Created by Team_5 on 2021/12/10.
//

#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;

class Stock // IDEA: set data member to be private, and use public functions to get access to data member
{
private:
    string ticker; //Every ticker attribute in EarningsAnnouncement
    string date; //EAD
    string period_end;
    double estimate_eps;
    double reported_eps;
    double surprise;
    double surprise_percent; //in a percent form, only number would be displayed
    int group;

    int day0index;
    string startdate;
    int startdateindex;
    int enddateindex;

    vector<string> dates;
    vector<double> prices;
    vector<double> benchmark_prices;
    vector<double> returns;
    vector<double> cumreturns;
    vector<double> abnormalreturns;

public:
    Stock() {}
    Stock(string ticker_) { ticker = ticker_; }

    string GetTicker() { return ticker; }
    int GetGroup() { return group; }
    int GetDay0Index() { return day0index; }
    string GetDate() { return date; }
    vector<string> GetDates() { return dates; }
    vector<double> GetPrices() { return prices; }
    vector<double> GetRet() { return returns; }
    vector<double> GetAR() { return abnormalreturns; }

    void SetTicker(string ticker_) { ticker = ticker_; }
    void SetDate(string date_) { date = date_; }
    void SetPeriodEnd(string period_end_) { period_end = period_end_; }
    void SetEstimateEPS(double estimate_eps_) { estimate_eps = estimate_eps_; }
    void SetReportedEPS(double reported_eps_) { reported_eps = reported_eps_; }
    void SetSurprise(double surprise_) { surprise = surprise_; }
    void SetSurprisePercent(double surprise_percent_) { surprise_percent = surprise_percent_; }
    void SetGroup(int group_) { group = group_; }

    void SetDay0Index(int day0index_) { day0index = day0index_; }
    void SetStartDate(string startdate_) { startdate = startdate_; }
    void SetStartDateIndex(int startdateindex_) { startdateindex = startdateindex_; }

    void SetDates(vector<string> dates_) { dates = dates_; }
    void SetPrices(vector<double> prices_) { prices = prices_; }
    void SetBenchmarkPrices(vector<double> benchmark_prices_) { benchmark_prices = benchmark_prices_; }

    // Calculate ret and abnormalreturns
    void CalculateRet();
    void CalculateAR();

    void display(int N);

};


