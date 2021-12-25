//
//  main.cpp
//  FRE6883_FinalProject
//
//  Created by Team_5 on 2021/11/24.
//

#include <iostream>
#include <map>
#include "Stock.hpp"
#include "ReadExcel.hpp"
#include "Libcurl_data.hpp"
#include "Bootstrap.hpp"
#include "Gnuplot_MAC.hpp"
using namespace std;

typedef vector<vector<string>> Group;

int main()
{
    cout << "Welcome to Russell1000 Earnings Surprise Impact Detecting System!" << endl << endl;
    cout << "#########################################################################" << endl;
    cout << "##    ####    ####   ###########  ####     ####       ########       ##" << endl;
    cout << "##    ####    ####   ###########  ####     ####      ####  ####      ##" << endl;
    cout << "##    ####    ####   ####         ####     ####     ####    ####     ##" << endl;
    cout << "##    ############   ###########  ####     ####    ####      ####    ##" << endl;
    cout << "##    ############   ###########  ####     ####    ####      ####    ##" << endl;
    cout << "##    ####    ####   ####         ####     ####     ####    ####     ##" << endl;
    cout << "##    ####    ####   ###########  #######  #######   ####  ####      ##" << endl;
    cout << "##    ####    ####   ###########  #######  #######    ########       ##" << endl;
    cout << "#######################################################################" << endl << endl;
    cout << "Press any button to start loading Russell 1000 stock data from csv file...." << endl;
    getchar();
    
    
    // Initialization
    map<string, Stock*> stocks = ReadExcel(EOD_FILE); // AnnounceDate: stockid, annoucement date  // Attention: remember to free pointers in the end
    Stock* benchmark = new Stock("IWB");
    map<string, double> benchmark_map;
    Bootstrap bts;
    vector<Matrix> Result;
    Group grouplist;
    int option;
    string soption;
    int number;
    string snumber;
    bool cont = true;
    string ticker;
    int group;
    string sgroup;
    vector<double> X;
    vector<double> CAAR1;
    vector<double> CAAR2;
    vector<double> CAAR3;

    cout << "Finish loading stock data from csv file!" << endl;
    cout << endl;
    cout << "Press any button to downloading Russell 1000 stock price data...." << endl;
    getchar();
    
    // Fetch all data
    benchmark_map = fetch_benchmark_price(benchmark);
    //fetch_stock_price(stocks, benchmark_map);
    fetch_stock_price_multithread(stocks, benchmark_map);
    datasanitycheck(stocks);

    // Calculate returns and abnormal returns for every stock for all time
    for (map<string, Stock*> ::iterator itr = stocks.begin(); itr != stocks.end(); itr++)
    {
        itr->second->CalculateRet();
        itr->second->CalculateAR();
    }
    
    cout << "The historical data of Russell 1000 stocks from 2020-10-01 to 2021-11-30 and the benchmark IWB has been downloaded."<< endl << endl << endl;
    

    // MENU
    
    while (cont) {
        cout << "Welcome to the menu!" << endl;
        cout << "Please input the number 1,2,3,4,5 to continue: " << endl;
        cout << "1: Enter N to retrieve 2N+1 days of historical price data for all stocks (60<=N<=120)" << endl;
        cout << "2: Pull information for one stock from one group" << endl;
        cout << "3: Show AAR, AAR-STD, CAAR and CAAR-STD for one group" << endl;
        cout << "4: Show the gnuplot graph with CAAR for all 3 groups" << endl;
        cout << "5: Exit" << endl << endl;
        cout << "Enter your choice: ";
        cin >> soption;
        getchar();
        cout << endl << endl << endl;
        
        while ((soption.at(0) > '5' || soption.at(0) <= '0'))
        {
            cout << "Invalid input! Try Again!" << endl << endl;
            cout << "1: Enter N to retrieve 2N+1 days of historical price data for all stocks (60<=N<=120)" << endl;
            cout << "2: Pull information for one stock from one group" << endl;
            cout << "3: Show AAR, AAR-STD, CAAR and CAAR-STD for one group" << endl;
            cout << "4: Show the gnuplot graph with CAAR for all 3 groups" << endl;
            cout << "5: Exit" << endl << endl;
            cout << "Enter your choice: ";
            cin >> soption;
            getchar();
            cout << endl << endl << endl;
        }

        option = stoi(soption); // change string to interger
        
        switch (option) {
        
        case 1:
            cout<<"Please set N (60<=N<=120): ";
            cin >> snumber;
            getchar();
            cout << endl << endl;
            if (snumber.at(0) > '9'){
                number = 60;
                bts.SetN(number);
                cout << "Invalid N, N has been automatically set to 60" << endl << endl;
            }
            else {
                number = stoi(snumber);
                if (number>=60 && number<=120) {
                bts.SetN(number);
                cout << "N has been set to " << number << endl << endl << endl;
                }
                else
                {
                number = 60;
                bts.SetN(number);
                cout << "Invalid N, N has been automatically set to 60" << endl << endl;
                }
            }
            // Erase stock without enough data
            for (map<string, Stock*> ::iterator itr = stocks.begin(); itr != stocks.end(); itr++)
            {
                int day0index = itr->second->GetDay0Index();
                int length = (int)itr->second->GetDates().size();
                if ((day0index <= bts.GetN()) or ((day0index + bts.GetN()) >= length)) {
                        stocks.erase(itr->second->GetTicker());
                }
            }
                
            // Divide groups and implement bootstrap
            grouplist = DivideGroup(stocks);
            Result = bts.finalCalculation(stocks, grouplist);
                
            break;
                
        case 2:
            cout << "Now you can pull information for one stock." << endl;
            cout << "Please enter stock ticker: ";
            cin >> ticker;
            getchar();
            cout << endl;
            while (!stocks[ticker]){
                cout << "Invalid ticker. Please enter a valid ticker: ";
                cin >> ticker;
                getchar();
                cout << endl;
            }
            stocks[ticker]->display(bts.GetN());
            cout << "Stock " << ticker << "'s informatoin has been shown." << endl << endl << endl;
            
            break;
                
        case 3:
            cout << "Please enter the group selection: " << endl
                 << "1. Beat group" << endl
                 << "2. Meet group" << endl
                 << "3. Miss group" << endl
                 << endl;
            cout << "Your choice: ";
            cin >> sgroup;
            getchar();
            cout << endl << endl;
            
            while ((sgroup.at(0) > '3' || sgroup.at(0) <= '0'))
            {
                cout << "Invalid input! Try Again!" << endl << endl;
                cout << "Please enter the group selection: " << endl
                     << "1. Beat group" << endl
                     << "2. Meet group" << endl
                     << "3. Miss group" << endl
                     << endl;
                cout << "Your choice: ";
                cin >> sgroup;
                getchar();
                cout << endl << endl;
            }
            
            group = stoi(sgroup); // transform into interger
            
            // Show AAR, AAR-STD, CAAR and CAAR-STD
            cout << "Index for Group " << group <<":" << endl << endl;
            cout << setw(5) << "t" // set the width of the output
                 << setw(14) << "AAR"
                 << setw(14) << "AAR-STD"
                 << setw(14) << "CAAR"
                 << setw(14) << "CAAR-STD"
                 << endl << endl;
            for (int i = -bts.GetN()+1; i <= bts.GetN(); i++) {
                cout << setw(5) << i
                     << setw(14) << Result[group-1][0][i+bts.GetN()-1]
                     << setw(14) << Result[group-1][1][i+bts.GetN()-1]
                     << setw(14) << Result[group-1][2][i+bts.GetN()-1]
                     << setw(14) << Result[group-1][3][i+bts.GetN()-1]
                     << endl << endl;
            }
            cout << endl;
                
            break;
                
        case 4:
            X.clear();
            for (int i = -bts.GetN()+1; i <= bts.GetN(); i++) {
                X.push_back(i);
            }
            CAAR1 = Result[0][2] - Result[0][2][bts.GetN()-1]; //shift three lines, minus the day0 cumreturn to show the trend before and after the announcement date
            CAAR2 = Result[1][2] - Result[1][2][bts.GetN()-1];
            CAAR3 = Result[2][2] - Result[2][2][bts.GetN()-1];
                
            cout << "The gnuplot graph with CAAR for all 3 groups has been shown." << endl << endl << endl;
            plotResults(X, CAAR1, CAAR2, CAAR3);

                    
            break;
                
        case 5:
            for (map<string, Stock*> ::iterator itr = stocks.begin(); itr != stocks.end(); itr++)
            {
                delete itr->second;
            }
            delete benchmark;
            cont = false;
            cout << "Program exit. Thank you."<< endl << endl;
            break;
        };
                
    };

    return 0;
    
}
