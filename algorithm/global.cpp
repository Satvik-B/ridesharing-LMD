/**
	\author:	Trasier
	\date:		2019/05/22
*/
#include "global.h"

int nR = 0;
int nW = 0;
request_t* requests = NULL;
worker_t* workers = NULL;
const double speed = 1.0;
const double EPS = 1e-5;
const double INF = 1e20;
double usedTime = 0.0;
int usedMemory = 0;

int dcmp(double x) {
	if (fabs(x) < EPS)
		return 0;
	return x>0 ? 1:-1;
}

double dist(vector<Coordinate>& V, int x, int y) { 
	Coordinate& a = V[x];
	Coordinate& b = V[y];
	return sqrt(1.0*(a.longitude - b.longitude)*(a.longitude - b.longitude) + 1.0*(a.latitude - b.latitude)*(a.latitude - b.latitude));
} 

double dist(Coordinate &a, Coordinate &b) {
	return sqrt(1.0*(a.longitude - b.longitude)*(a.longitude - b.longitude) + 1.0*(a.latitude - b.latitude)*(a.latitude - b.latitude));
}

void freeInput() {
	delete[] requests;
	delete[] workers;
}
