/**
	\Author: 	Trasier
	\Date:		2019/05/22
	\Note: 		A fast implementation of my draft for VLDB'20.
**/
#include <bits/stdc++.h>
#include "global.h"
#include "HST.h"
#include "utils.h"
using namespace std; 

#ifdef WATCH_MEM
#include "monitor.h"
#endif

int nP = 0;
int* reqPool;
int* reqPos;
const double step = 2.0;
double rho = 1, delta = rho;

void localInit() {
	delta = rho;
	
	reqPos = new int[nR];
	reqPool = new int[nR];
	for (int i=0; i<nR; ++i) {
		reqPool[i] = i;
		reqPos[i] = i;
	}
	nP = nR;
}

void localFree() {
	delete[] reqPos;
	delete[] reqPool;
}

vector<vector<int> > createResult();
void wrapper(vector<item> items, Coordinate warehouse, int numberOfRiders, Bin bin);

void budget(int wid, vector<int>& Rw, vector<int>& Sw, double delta) {
	worker_t& w = workers[wid];
	Rw.clear();
	Sw.clear();	
	vector<int> sorted;
	
	for (int i=0; i<nP; ++i) {
		request_t& r = requests[reqPool[i]];
		if (localDist(w.oid, r.oid)+localDist(r.oid, r.did) > delta)
			continue;
		sorted.push_back(reqPool[i]);
	}
	if (sorted.empty()) return ;
	
 	genLabel(w, H);
	genVec(sorted);
	sortPath(sorted);
	
	double tot = 0.0, inc;
	int oPos, dPos, rid;
	
	worker_t wtmp;
	wtmp.oid = w.oid;
	wtmp.cap = w.cap;
	wtmp.S.push_back(wtmp.oid);
	
	for (int i=0; i<sorted.size(); ++i) {
		rid = sorted[i];
		insertDist(wtmp, rid, oPos, dPos, inc);
		tot += inc;
		if (tot > delta) break;
		
		insert(wtmp, rid, oPos, dPos);
		Rw.push_back(rid);
	}
	Sw = wtmp.S;
}

void updateRoute(int wid, vector<int>& Rw, vector<int>& Sw) {
	worker_t& w = workers[wid];
	w.S.insert(w.S.end(), Sw.begin()+1, Sw.end());
	
	for (int i=0; i<Rw.size(); ++i) {
		int rid = Rw[i], pos = reqPos[rid];
		swap(reqPool[pos], reqPool[nP-1]);
		reqPos[reqPool[pos]] = pos;
		--nP;
	}
}

vector<int> getRequest(int wid) {
	worker_t& w = workers[wid];
	vector<int>& S = w.S;
	int n = S.size();
	vector<int> ret;
	
	for (int i=1; i<n; ++i) {
		if (S[i] & 1)
			ret.push_back(S[i]>>1);
	}
	
	return ret;
}

void FESI() {
	vector<pair<double,int> > vpi(nW, make_pair(0,0));
	delta = rho;

	localInit();
	
	int assigned = 0;
	vector<int> Rw, Sw;
	
	for (int i=0; i<nW; ++i) {
		vpi[i].second = i;
	}
	while (assigned < nR) {
		for (int i=0; i<nW; ++i) {
			int j = vpi[i].second;
			budget(j, Rw, Sw, delta);
			if (!Rw.empty()) {
				assigned += Rw.size();
				updateTree(Rw);
				updateRoute(j, Rw, Sw);
				if (assigned == nR)
					break;
			}
		}
		delta *= step;
		for (int i=0; i<nW; ++i) {
			vpi[i].first = getRouteDist(i);
			vpi[i].second = i;
		}
		sort(vpi.begin(), vpi.end());
	}
	
#ifdef WATCH_MEM
	watchSolutionOnce(getpid(), usedMemory);
#endif
}

int main(int argc, char** argv) {
	ifstream input;
	input.open("input.txt");
	Coordinate warehouse;
	input>>warehouse.longitude>>warehouse.latitude;
	Bin bin;
	input>>bin.size.length>>bin.size.width>>bin.size.height;
	bin.capacity = 100;
	int n;
	input>>n;
	vector<item> items(n);
	for(int i=0;i<n;i++) {
		input>>items[i].coordinate.longitude>>items[i].coordinate.latitude;
		input>>items[i].size.length>>items[i].size.width>>items[i].size.height;
		items[i].weight = 1;
	}

	wrapper(items, warehouse, 1, bin);
	freeGlobalMemory();
	localFree();
	
	return 0;
}

void wrapper(vector<item> items, Coordinate warehouse, int numberOfRiders, Bin bin) {
	set<Coordinate> all_coordinates;
	for(auto item : items) {
		all_coordinates.insert(item.coordinate);
	}
	all_coordinates.insert(warehouse);
	
	
	// initiate location vector
	nV = all_coordinates.size();
	nW = numberOfRiders;
	nR = items.size();
	initGlobalMemory(nV, nW, nR);
	int count = 0;
	for(auto coordinate:all_coordinates) V[count++] = coordinate;
	assert(count==nV);
	
	int warehouse_id = lower_bound(V.begin(), V.end(), warehouse) - V.begin();
	// ConstructHST
	constructHST(false);
	for(int i=0;i<nW;i++) {
		workers[i].oid = warehouse_id;
		workers[i].cap = bin;
	}

	for(int i=0;i<nR;i++) {
		requests[i].did = lower_bound(V.begin(), V.end(), items[i].coordinate) - V.begin();
		requests[i].oid = warehouse_id;
		requests[i].wei = items[i].weight;
		requests[i].volume = items[i].size.height*items[i].size.width*items[i].size.length;
	}


	nT = calcTreeSize();
	labels = new int[nT];
	memset(labels, 0, sizeof(int)*nT);
	// (5)
	mark = new int[nT];
	memset(mark, 0, sizeof(int)*nT);
	// (6)
	nodes = new node_t[nT];

	initWorkers();
	initNodes();
	initTree();
	initRng();

	FESI();

	vector<vector<int> > clusterId = createResult();

	vector<int> clusterCount(nR);
	for(int i=0;i<clusterId.size();i++) {
		for(int j=0;j<clusterId[i].size();j++) clusterCount[clusterId[i][j]]=i;
	}

	ofstream myfile;
  	myfile.open ("output.txt");

	for(int i=0;i<clusterCount.size();i++) {
		myfile<<clusterCount[i]<<" ";
	}
	myfile<<endl;
	myfile.close();
}

vector<vector<int> > createResult() {
	vector<vector<int> > clusterId;
	for(int j=0; j<nW; j++){
		double sf = 0, mf = 0, st = 0, mt = 0, tmp;
		vector<int> &S = workers[j].S;
		int _pid = workers[j].oid, pid;
		vector<int> tempCluster;
		for(int i=1; i<S.size(); ++i){
			pid = (S[i]&1) ? requests[S[i]>>1].did : requests[S[i]>>1].oid;
			tmp = localDist(_pid, pid);
			mt += tmp;
			if (S[i] & 1) {	
				tempCluster.push_back(S[i]>>1);
				sf += mt;
				mf = max(mf, mt);
			} else{
				if(!tempCluster.empty()) {
					clusterId.push_back(tempCluster);
					tempCluster.clear();
				}
				tempCluster.clear();
			}
			_pid = pid;
		}	
		if(!tempCluster.empty()) {
			clusterId.push_back(tempCluster);
			tempCluster.clear();
		}
	}
	return clusterId;
}