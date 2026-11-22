#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<iomanip>
#include<sstream>
#include "../LRU-Optimal/LRU.h"
#include "../FIFO-LFU/FIFO.h"
#include "../FIFO-LFU/LFU.h"
#include "../LRU-Optimal/optimal.h"
using namespace std;

struct PolicyResult{
  string name;
  int hits;
  int misses;
  int removals;
  double hitRatio;
  double missRatio;
};

class Simulator{
public:

  void runSingle(const string& policy,int capacity,const vector<int>& requests){
    cout<<"\n=== "<<policy<<" step by step (capacity "<<capacity<<") ===\n";
    if(policy=="LRU"){ LRU c; c.runLRU(capacity,requests,true); }
    else if(policy=="FIFO"){ FIFO c; c.runFIFO(capacity,requests,true); }
    else if(policy=="LFU"){ LFU c; c.runLFU(capacity,requests,true); }
    else if(policy=="Optimal"){ Optimal c; c.runOptimal(capacity,requests,true); }
    else cout<<"Unknown policy: "<<policy<<"\n";
  }

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

    Optimal opt;
    opt.runOptimal(capacity,requests,false,false);
    results.push_back({"Optimal",opt.getHits(),opt.getMisses(),opt.getRemovals(),
                       opt.getHitRatio(),opt.getMissRatio()});

    return results;
  }
  void printComparison(int capacity,const vector<int>& requests){
    vector<PolicyResult> results=compare(capacity,requests);

    cout<<"\n=== Comparison (capacity "<<capacity<<", "<<requests.size()<<" requests) ===\n";
    cout<<left<<setw(9)<<"Policy"<<setw(8)<<"Hits"<<setw(8)<<"Misses"
        <<setw(12)<<"Hit ratio"<<setw(12)<<"Miss ratio"<<"Replacements\n";
    for(const PolicyResult& r : results){
      cout<<left<<setw(9)<<r.name<<setw(8)<<r.hits<<setw(8)<<r.misses
          <<setw(12)<<(percent(r.hitRatio)+"%")
          <<setw(12)<<(percent(r.missRatio)+"%")
          <<r.removals<<"\n";
    }

    int bestHits=-1;
    for(const PolicyResult& r : results)
      if(r.name!="Optimal" && r.hits>bestHits) bestHits=r.hits;
    string best="";
    for(const PolicyResult& r : results){
      if(r.name!="Optimal" && r.hits==bestHits){
        if(!best.empty()) best+=" and ";
        best+=r.name;
      }
    }
    cout<<"Best real policy for this input: "<<best<<"\n";
    cout<<"Optimal is the best possible result (it knows the future requests).\n";
  }

  void printCapacityTable(int maxCapacity,const vector<int>& requests){
    cout<<"\n=== Hit ratio (%) for different capacities ===\n";
    cout<<left<<setw(10)<<"Capacity"<<setw(10)<<"LRU"<<setw(10)<<"FIFO"
        <<setw(10)<<"LFU"<<"Optimal\n";
    for(int c=1;c<=maxCapacity;c++){
      vector<PolicyResult> r=compare(c,requests);
      cout<<left<<setw(10)<<c;
      for(int i=0;i<(int)r.size();i++){
        string text=percent(r[i].hitRatio);
        if(i<(int)r.size()-1) cout<<setw(10)<<text; else cout<<text;
      }
      cout<<"\n";
    }
  }

private:

  string percent(double x){
    ostringstream out;
    out<<fixed<<setprecision(2)<<x;
    return out.str();
  }
};
