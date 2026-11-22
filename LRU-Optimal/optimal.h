#pragma once
#include<iostream>
#include<unordered_map>
#include<queue>
#include<vector>
#include<algorithm>
#include<climits>
#include<iomanip>
using namespace std;

class Optimal{
  unordered_map<int,queue<int>> future;
  unordered_map<int,int> cache;      
  int Capacity=0;
  int hits=0, misses=0, removals=0, total=0;
  bool removedAny=false;           
  int lastRemoved=0;                 

  int nextUse(int key){
    if(future[key].empty()) return INT_MAX;
    return future[key].front();
  }

  void display(){
    vector<pair<int,int>> items;     
    for(auto& it : cache) items.push_back({nextUse(it.first),it.first});
    sort(items.begin(),items.end(),[](const pair<int,int>& a,const pair<int,int>& b){
      if(a.first!=b.first) return a.first>b.first;
      return a.second<b.second;
    });
    cout<<"cache: ";
    for(auto& p : items) cout<<p.second<<" ";
    cout<<"\n";
  }

  void put(int key){
    if(Capacity==0) return;
    if((int)cache.size()==Capacity){
   
      int victim=-1, farthest=-1;
      for(auto& it : cache){
        int n=nextUse(it.first);
        if(n>farthest || (n==farthest && it.first<victim)){
          farthest=n;
          victim=it.first;
        }
      }
      lastRemoved=victim;
      removedAny=true;
      removals++;
      cache.erase(victim);
    }
    cache[key]=key;
  }

  bool get(int key){
    removedAny=false;
    if(cache.find(key)==cache.end()){
      misses++;
      return false;
    }
    hits++;
    return true;
  }

public:
  void runOptimal(int capacity,const vector<int>& requests,bool show,bool printSummary=true){
    future.clear();
    cache.clear();
    hits=0; misses=0; removals=0; removedAny=false;
    total=(int)requests.size();
    Capacity=(capacity<0)?0:capacity;

    for(int i=0;i<total;i++) future[requests[i]].push(i);

  
    for(int x : requests){
      future[x].pop();               
      if(get(x)){ if(show) cout<<x<<" HIT  "; }
      else { put(x); if(show) cout<<x<<" MISS "; }
      if(show){
        cout<<"removed: ";
        if(removedAny) cout<<lastRemoved; else cout<<"-";
        cout<<" | ";
        display();
      }
    }
    if(printSummary){
      cout<<fixed<<setprecision(2);
      cout<<"Capacity: "<<capacity<<" | Hits: "<<hits<<" Misses: "<<misses
          <<" Hit ratio: "<<getHitRatio()<<"% Miss ratio: "<<getMissRatio()
          <<"% Replacements: "<<removals<<"\n";
    }
    future.clear();
    cache.clear();
  }

  int getHits() const { return hits; }
  int getMisses() const { return misses; }
  int getRemovals() const { return removals; }
  int getTotal() const { return total; }
  double getHitRatio() const { return total ? 100.0*hits/total : 0.0; }
  double getMissRatio() const { return total ? 100.0*misses/total : 0.0; }
};
