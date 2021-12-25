//
//  Gnuplot_MAC.cpp
//  FRE6883_FinalProject
//
//  Created by Team_5 on 2021/12/11.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Gnuplot_MAC.hpp"

using namespace std;

void plotResults(vector<double> xData, vector<double> yData, vector<double> yData2, vector<double> yData3)
{
    FILE *gnuplotPipe,*tempDataFile;
    const char *tempDataFileName1 = "Beat";
    const char *tempDataFileName2 = "Meet";
    const char *tempDataFileName3 = "Miss";
    double x, y, x2, y2, x3, y3;
    int i;
    int dataSize = (int)xData.size() - 1;
    gnuplotPipe = popen("/opt/local/bin/gnuplot","w");
    if (gnuplotPipe)
    {
        fprintf(gnuplotPipe, "set term wxt\n"); // set to interactive mode wxt
        fprintf(gnuplotPipe, "set grid\n"); // grid
        fprintf(gnuplotPipe, "set xlabel 'N - Number of days'\n");
        fprintf(gnuplotPipe, "set ylabel 'CAAR Mean'\n");
        fprintf(gnuplotPipe, "set title 'CAAR Mean of all groups'\n");
        fprintf(gnuplotPipe,"plot \"%s\" with lines, \"%s\" with lines, \"%s\" with lines\n",tempDataFileName1, tempDataFileName2, tempDataFileName3); // add legend
        
        fflush(gnuplotPipe); // flush the useless elements in gnuplotPipe stream
        
        tempDataFile = fopen(tempDataFileName1,"w");
        for (i=0; i <= dataSize; i++) {
            x = xData[i];
            y = yData[i];
            fprintf(tempDataFile,"%lf %lf\n",x,y);
        }
        fclose(tempDataFile);
        
        tempDataFile = fopen(tempDataFileName2,"w");
        for (i=0; i <= dataSize; i++) {
            x2 = xData[i];
            y2 = yData2[i];
            fprintf(tempDataFile,"%lf %lf\n",x2,y2);
        }
        fclose(tempDataFile);
        
        tempDataFile = fopen(tempDataFileName3,"w");
        for (i=0; i <= dataSize; i++) {
            x3 = xData[i];
            y3 = yData3[i];
            fprintf(tempDataFile,"%lf %lf\n",x3,y3);
        }
        fclose(tempDataFile);
        
        printf("press enter to continue...");
        getchar(); //get a single character from the input command line

        remove(tempDataFileName1);
        remove(tempDataFileName2);
        remove(tempDataFileName3);
        fprintf(gnuplotPipe,"exit \n");
    }
    else
    {
        printf("gnuplot not found...");
    }

}


