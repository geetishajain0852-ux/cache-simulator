#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<iomanip>
#include<sstream>
#include "../LRU-Optimal/LRU.h"
#include "../FIFO-LFU/FIFO.h"
#include "../FIFO-LFU/LFU.h"
using namespace std;

// Holds the final numbers of ONE policy after a run.
// The UI can use this directly (for tables and charts).
struct PolicyResult{
  string name;
  int hits;
  int misses;
  int removals;
  double hitRatio;
  double missRatio;
};

// The Simulator runs the SAME requests through LRU, FIFO and LFU
// and compares them.
class Simulator{
public:

  // 1) Run ONE policy and show every step (hit/miss, removed item, cache).
  //    policy must be "LRU", "FIFO" or "LFU".
  void runSingle(const string& policy,int capacity,const vector<int>& requests){
    cout<<"\n=== "<<policy<<" step by step (capacity "<<capacity<<") ===\n";
    if(policy=="LRU"){ LRU c; c.runLRU(capacity,requests,true); }
    else if(policy=="FIFO"){ FIFO c; c.runFIFO(capacity,requests,true); }
    else if(policy=="LFU"){ LFU c; c.runLFU(capacity,requests,true); }
    else cout<<"Unknown policy: "<<policy<<"\n";
  }

  // 2) Run all three silently and return their results.
  vector<PolicyResult> compare(int capacity,const vector<int>& requests){
    vector<PolicyResult> results;

    LRU lru;
    lru.runLRU(capacity,requests,false,false);
    results.push_back({"LRU",lru.getHits(),lru.getMisses(),lru.getRemovals(),
                       lru.getHitRatio(),lru.getMissRatio()});

    FIFO fifo;
    fifo.runFIFO(capacity,requests,false,false);
    results.push_back({"FIFO",fifo.getHits(),fifo.getMisses(),fifo.getRemovals(),
                       fifo.getHitRatio(),fifo.getMissRatio()});

    LFU lfu;
    lfu.runLFU(capacity,requests,false,false);
    results.push_back({"LFU",lfu.getHits(),lfu.getMisses(),lfu.getRemovals(),
                       lfu.getHitRatio(),lfu.getMissRatio()});

    return results;
  }

  // 3) Print a comparison table for one capacity and name the best policy.
  void printComparison(int capacity,const vector<int>& requests){
    vector<PolicyResult> results=compare(capacity,requests);

    cout<<"\n=== Comparison (capacity "<<capacity<<", "<<requests.size()<<" requests) ===\n";
    cout<<left<<setw(8)<<"Policy"<<setw(8)<<"Hits"<<setw(8)<<"Misses"
        <<setw(12)<<"Hit ratio"<<setw(12)<<"Miss ratio"<<"Replacements\n";
    for(const PolicyResult& r : results){
      cout<<left<<setw(8)<<r.name<<setw(8)<<r.hits<<setw(8)<<r.misses
          <<setw(12)<<(percent(r.hitRatio)+"%")
          <<setw(12)<<(percent(r.missRatio)+"%")
          <<r.removals<<"\n";
    }

    // find the best policy (most hits); if several tie, list them all
    int bestHits=-1;
    for(const PolicyResult& r : results) if(r.hits>bestHits) bestHits=r.hits;
    string best="";
    for(const PolicyResult& r : results){
      if(r.hits==bestHits){
        if(!best.empty()) best+=" and ";
        best+=r.name;
      }
    }
    cout<<"Best policy for this input: "<<best<<"\n";
  }

  // 4) Print hit ratio of every policy for capacities 1 to maxCapacity.
  //    This shows how a bigger cache gives more hits.
  void printCapacityTable(int maxCapacity,const vector<int>& requests){
    cout<<"\n=== Hit ratio (%) for different capacities ===\n";
    cout<<left<<setw(10)<<"Capacity"<<setw(10)<<"LRU"<<setw(10)<<"FIFO"<<"LFU\n";
    for(int c=1;c<=maxCapacity;c++){
      vector<PolicyResult> r=compare(c,requests);
      cout<<left<<setw(10)<<c;
      for(int i=0;i<(int)r.size();i++){
        string text=percent(r[i].hitRatio);
        if(i<2) cout<<setw(10)<<text; else cout<<text;
      }
      cout<<"\n";
    }
  }

private:
  // turns 33.3333 into "33.33" (two decimal places)
  string percent(double x){
    ostringstream out;
    out<<fixed<<setprecision(2)<<x;
    return out.str();
  }
};
