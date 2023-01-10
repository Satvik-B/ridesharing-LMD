#ifndef FESIF_H
#define FESIF_H


#include <bits/stdc++.h>
#include "global.h"
#include "HST.h"
#include "utils.h"
using namespace std; 

int nP = 0;
int* reqPool;
int* reqPos;
const double step = 2.0;
double rho = 1, delta = rho;

void localInit();
void localFree();
void budget(int wid, vector<int>& Rw, vector<int>& Sw, double delta);
void updateRoute(int wid, vector<int>& Rw, vector<int>& Sw);
vector<int> getRequest(int wid);
void FESI();



vector<vector<int> > createResult();
void wrapperLMD(vector<item>& items, Coordinate warehouse, int numberOfRiders, Bin bin);


#endif