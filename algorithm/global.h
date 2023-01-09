/**
	\author:	Trasier
	\date:		2019/05/22
*/
#ifndef GLOBAL_H
#define GLOBAL_H

#include <bits/stdc++.h>
using namespace std;

struct Dimension {
    float length, width, height;

    Dimension(){    }
    Dimension(float l, float w, float h)
    {
        length = l;
        width = w;
        height = h;
    }
};


struct Position {
    float x, y, z;
};

struct Coordinate {
    float longitude, latitude;
    Coordinate(){}
    Coordinate(float lat, float lng)
    {
        longitude = lng;
        latitude = lat;
    }
};

inline bool operator<(const Coordinate& a1, const Coordinate& a2)
{
    if (a1.longitude != a2.longitude) return a1.longitude < a2.longitude;
    if (a1.latitude != a2.latitude) return a1.latitude < a2.latitude;
    return false;
} 

struct PolarCoordinate {
    float radius, angle;
};

struct item {
    string id;
    Dimension size;
    Position position;
    Coordinate coordinate;
    PolarCoordinate polarCoordinate;
    float weight;
    item(){}
    item(float w, float h, float d, float lat, float lng)
    {
        size.width = w;
        size.height = h;
        size.length = d;
        coordinate.latitude = lat;
        coordinate.longitude = lng;
        weight = 0;
    }

} ;

class Bin{
    public:
    Dimension size;
    float capacity;
    Bin(){}
    Bin(float w,float l, float h){
        size.width = w;
        size.length = l;
        size.height = h;
        capacity = 1000;
    }

    Bin(float w, float l, float h, float c){
        size.width = w;
        size.length = l;
        size.height = h;
        capacity = c;
    }

    float getVolume(){
        return size.width * size.length * size.height * 0.85;
    }
};


// typedef unorderd_map<int,int> hii;
typedef long long LL;
typedef pair<int,int> pii;
typedef pair<double,double> pdd;
typedef pair<int,double> pid;
typedef pair<double,int> pdi;

typedef struct {
    int oid, did; 	// origin and destination
    double wei; 
    double volume;		// weight
} request_t;

typedef struct {
    int oid;		// initial location
    Bin cap;		// capacity
	vector<int> S;
} worker_t;


extern int nR;
extern int nW;
extern request_t* requests;
extern worker_t* workers;
extern const double speed;
extern const double EPS;
extern const double INF;
extern double usedTime;
extern int usedMemory;

int dcmp(double x);
double dist(vector<Coordinate>& V, int x, int y);
double dist(Coordinate& a, Coordinate& b);
void freeInput();
#endif
