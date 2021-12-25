//
//  Libcurl_data.hpp
//  FRE6883_FinalProject
//
//  Created by Team_5 on 2021/12/10.
//

#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <locale>
#include <iomanip>
#include "curl/curl.h"
#include "Stock.hpp"
#include <thread>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <time.h>
using namespace std;

struct MemoryStruct
{
    char* memory;
    size_t size;
};

void* myrealloc(void* ptr, size_t size);
size_t write_data(void* ptr, size_t size, size_t nmemb, void* data);
map<string, double> fetch_benchmark_price(Stock* benchmark); // fetch benchmark price first
map<string, vector<double>> fetch_stock_price(map<string, Stock*>& stocks, map<string, double> benchmark_map);
void fetch_stock_price_multithread(map<string, Stock*>& stocks, map<string, double> benchmark_map);
int thread_block(map<string, double> benchmark_map);
void datasanitycheck(map<string, Stock*>& stocks);
