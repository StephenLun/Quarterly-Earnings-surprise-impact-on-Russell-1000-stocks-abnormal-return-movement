//
//  ReadExcel.hpp
//  FRE6883_FinalProject
//
//  Created by Team_5 on 2021/12/10.
//

#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include "Stock.hpp"
//#define EOD_FILE ".\\EarningsAnnouncements.csv" // Windows version
#define EOD_FILE "EarningsAnnouncements.csv" // Mac version
using namespace std;

map<string,Stock*> ReadExcel(string filename);
