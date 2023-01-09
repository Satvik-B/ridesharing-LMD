/**
	\Author: Trasier
	\Date:	2019/05/21
**/

#include "HST.h"
#include "global.h"
#include<bits/stdc++.h>  

const int MAX_NODE = 100007;
const int MAX_HEIGHT = 21;
const int MAX_SAMPLE = 100;

int nV = 0, H = 0;
vector<Coordinate> V;
vector<int> pi;
int *reverse_pi = NULL;
int **far = NULL;
double dmax = -1.0;
double gama = -1.0;
double *exp2s = NULL;
double *sum2s = NULL;
static vector<int> pi_bk;
static double gamma_bk = -1.0;
std::random_device rd;
std::mt19937 g(rd());


void initMemory(int n) {
	nV = n;
	V.resize(nV);
	pi.resize(nV);
	pi_bk.resize(nV);
	reverse_pi = new int[nV];
	far = new int*[nV];
	for (int i=0; i<nV; ++i)
		far[i] = new int[MAX_HEIGHT];
	exp2s = new double[MAX_HEIGHT];
	exp2s[0] = 1.0;
	for (int i=1; i<MAX_HEIGHT; ++i)
		exp2s[i] = exp2s[i-1] * 2.0;
	
	sum2s = new double[MAX_HEIGHT+1];
	sum2s[0] = 0.0;
	for (int i=1; i<=MAX_HEIGHT; ++i)
		sum2s[i] = sum2s[i-1] + exp2s[i];
}

void freeMemory() {
	// delete[] V;
	// delete[] pi;
	delete[] reverse_pi;
	for (int i=0; i<nV; ++i)
		delete[] far[i];
	delete[] far;
	delete[] exp2s;
	delete[] sum2s;
}

void initLocation(string &fileName) {
	ifstream fin(fileName.c_str(), ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName.c_str());
		exit(1);
	}
	
	fin >> nV;
	initMemory(nV);
    for (int i=0; i<nV; ++i) {
        fin >> V[i].longitude;
        fin >> V[i].latitude;
    }
    fin.close();
}

void calcDmax() {
	if (dmax >= 0.0) return ;
	
	// initialize the parameters for decomposition
	dmax = 0.0;
	for (int i=0; i<nV; i++) {
		for (int j=i+1; j<nV; j++) {
			dmax = max(dmax, dist(V, i, j));
			#ifdef LOCAL_DEBUG
			printf("dist(%d,%d) = %.2lf\n", i, j, dist(V, i, j));
			#endif
		}
	}
	
	#ifdef LOCAL_DEBUG
	printf("dmax = %.2lf\n", dmax);
	#endif
}

void randomization() {
	// generate the permutation pi
	for (int i=0; i<nV; ++i){
		pi[i] = i;
	}
	std::shuffle(pi.begin(), pi.end(), g);
	for (int i=0; i<nV; ++i){
		reverse_pi[pi[i]] = i;
	}
	// generate the parameter gama
	gama = rand()%10000 + 5000;
	gama = min(0.9999999, round(gama*10000)/10000.0);
}

double distOnHST(int u, int v) {
	int level = levelOfLCA(u, v);
	return distAtLevel(level);
}

double distAtLevel(int level) {
	return sum2s[level] * gama * 2.0;
}

int levelOfLCA(int u, int v) {
	for (int i=0; i<H; ++i) {
		if(far[u][i] == far[v][i]) {
			return i;
		}
	}
	
	return H;
}

pair<int,int> getLCA(int u, int v) {
	for (int i=0; i<H; ++i) {
		if(far[u][i] == far[v][i]) {
			return make_pair(far[u][i], i);
		}
	}
	
	return make_pair(far[u][H], H);
}

void printHST() {
	printf("An HST is constructed:\n");
	printf("nV = %d, H = %d, gama = %.2lf\n", nV, H, gama);
	for (int i=0; i<nV; ++i){
		printf("%d: ", i);
		for (int j=0; j<=H; ++j){
			printf("%d ", far[i][j]);
		}
		printf("\n");
	}
}

void dumpHST(string& fileName, bool flag) {
	fstream fout(fileName.c_str(), ios::out);   
    
	if (!fout.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName.c_str());
		exit(1);
	}
	
	// fprintf parameters: nV, H, gama, 
	fout << nV << " " << H << " " << gama << endl;
	// fprintf fa:
	for (int i=0; i<nV; ++i){
		for (int j=0; j<=H; ++j){
			fout << far[i][j] << " ";
		}
		fout << endl;
	}
	
	// fprintf permutations: 
	if (flag) {
		for (int i=0; i<nV; ++i)
			fout << pi[i] << " ";
		fout << endl;
	}
	fout.close();
}

void readHST(string& fileName, bool flag) {
	fstream fin(fileName.c_str(), ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName.c_str());
		exit(1);
	}
	
	fin >> nV >> H >> gama;
	for (int i=0; i<nV; ++i){
		for (int j=0; j<=H; ++j){
			fin >> far[i][j];
		}
	}
	
	if (flag) {
		for (int i=0; i<nV; ++i)
			fin >> pi[i];
	}
	fin.close();
}

void backupHST() {
	gamma_bk = gama;
	pi_bk = pi;
}

void loadHST() {
	gama = gamma_bk;
	pi_bk = pi;
	for (int i=0; i<nV; ++i){
		reverse_pi[pi[i]] = i;
	}
}

void constructHST(bool load) {
	if (!load)
		randomization();
	calcDmax();
	
	// initialization
	H = ceil(log2(dmax));	
	for (int i=0; i<nV; ++i){
		for (int j=0; j<=H; ++j){	
			if (j == H) {
				far[i][j] = 0;
			} else {
				far[i][j] = reverse_pi[i];
			}
		}
	}
	
	vector<vector<int> > preC, curC;
	vector<int> vtmp;
	for (int i=0; i<nV; ++i)
		vtmp.push_back(i);
	preC.push_back(vtmp);
	vtmp.clear();
	
	// construct the HST by brute-force
	int nid = 1;
	
	for (int i=H-1; i>=0; --i) {
		double li = gama * exp2s[i];
		for (int cid=0; cid<preC.size(); ++cid) {
			vector<int> cluster = preC[cid];
			for (int j=0; j<nV; ++j) {
				vector<int> newCluster;
				for (int uid=cluster.size()-1; uid>=0; --uid) {
					int u = cluster[uid];
					if (dist(V, u, pi[j]) < li) {
						newCluster.push_back(u);
						cluster[uid] = *cluster.rbegin();
						cluster.pop_back();
						// construct the array of far
						far[u][i] = nid;
					}
				}
				if (!newCluster.empty()) {
					curC.push_back(newCluster);
					++nid;
				}
			}
		}
		preC = curC;
		curC.clear();
	}
}
