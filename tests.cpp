#include "simulator/Simulator.h"
#include <algorithm>
#include <random>
using namespace std;

int passed=0, failed=0;

void check(const string& name,bool ok){
  if(ok){ passed++; cout<<"PASS  "<<name<<"\n"; }
  else  { failed++; cout<<"FAIL  "<<name<<"\n"; }
}


struct Expected{
  int hits;
  int removals;
};

void expectAll(const string& name,int capacity,const vector<int>& requests,
               Expected lru,Expected fifo,Expected lfu,Expected opt){
  Simulator sim;
  vector<PolicyResult> r=sim.compare(capacity,requests);  
  Expected want[4]={lru,fifo,lfu,opt};
  for(int i=0;i<4;i++){
    bool ok = r[i].hits==want[i].hits
           && r[i].removals==want[i].removals
           && r[i].hits+r[i].misses==(int)requests.size();   
    check(name+" - "+r[i].name,ok);
    if(!ok){
      cout<<"      expected hits "<<want[i].hits<<", removals "<<want[i].removals
          <<" | got hits "<<r[i].hits<<", misses "<<r[i].misses
          <<", removals "<<r[i].removals<<"\n";
    }
  }
}


int simpleLRU(int cap,const vector<int>& req){
  vector<int> cache;                      
  int hits=0;
  for(int x : req){
    auto it=find(cache.begin(),cache.end(),x);
    if(it!=cache.end()){ hits++; cache.erase(it); cache.push_back(x); }
    else if(cap>0){
      if((int)cache.size()==cap) cache.erase(cache.begin());
      cache.push_back(x);
    }
  }
  return hits;
}

int simpleFIFO(int cap,const vector<int>& req){
  vector<int> cache;                      
  int hits=0;
  for(int x : req){
    if(find(cache.begin(),cache.end(),x)!=cache.end()) hits++;
    else if(cap>0){
      if((int)cache.size()==cap) cache.erase(cache.begin());
      cache.push_back(x);
    }
  }
  return hits;
}

int simpleLFU(int cap,const vector<int>& req){
  vector<int> keys, count, lastUsed;       
  int hits=0, time=0;
  for(int x : req){
    time++;
    int pos=-1;
    for(int i=0;i<(int)keys.size();i++) if(keys[i]==x) pos=i;
    if(pos!=-1){ hits++; count[pos]++; lastUsed[pos]=time; }
    else if(cap>0){
      if((int)keys.size()==cap){
        int victim=0;                      
        for(int i=1;i<(int)keys.size();i++){
          if(count[i]<count[victim] || (count[i]==count[victim] && lastUsed[i]<lastUsed[victim]))
            victim=i;
        }
        keys.erase(keys.begin()+victim);
        count.erase(count.begin()+victim);
        lastUsed.erase(lastUsed.begin()+victim);
      }
      keys.push_back(x); count.push_back(1); lastUsed.push_back(time);
    }
  }
  return hits;
}

int simpleOptimal(int cap,const vector<int>& req){
  vector<int> cache;
  int hits=0;
  for(int i=0;i<(int)req.size();i++){
    int x=req[i];
    if(find(cache.begin(),cache.end(),x)!=cache.end()){ hits++; continue; }
    if(cap==0) continue;
    if((int)cache.size()==cap){
      int victim=0, farthest=-1;
      for(int k=0;k<(int)cache.size();k++){
        int next=(int)req.size();        
        for(int j=i+1;j<(int)req.size();j++) if(req[j]==cache[k]){ next=j; break; }
        if(next>farthest){ farthest=next; victim=k; }
      }
      cache.erase(cache.begin()+victim);
    }
    cache.push_back(x);
  }
  return hits;
}

int main(){
  cout<<"===== Tests for the Cache Replacement Simulator =====\n\n";

  cout<<"--- Hand-checked examples ---\n";


  expectAll("Sample input (cap 3)",3,{1,2,3,1,4,1,5,1,2,3},
            {3,4},{2,5},{3,4},{4,3});

.
  expectAll("FIFO beats LRU (cap 2)",2,{1,2,1,3,2},
            {1,2},{2,1},{1,2},{2,1});

  expectAll("LFU keeps the popular item (cap 2)",2,{1,1,2,3,1},
        {1,2},{1,2},{2,1},{2,1});
  expectAll("LFU tie-break (cap 2)",2,{1,2,3,2},
            {1,1},{1,1},{1,1},{1,1});


  cout<<"\n--- Edge cases ---\n";


  expectAll("Capacity 0",0,{1,2,1,2},
            {0,0},{0,0},{0,0},{0,0});

  expectAll("Capacity 1",1,{1,1,2,2,1},
            {2,2},{2,2},{2,2},{2,2});

  expectAll("Empty request list",3,{},
            {0,0},{0,0},{0,0},{0,0});


  expectAll("Big cache (cap 5)",5,{1,2,1,2,1,2},
            {4,0},{4,0},{4,0},{4,0});


  expectAll("Same item repeated (cap 2)",2,{7,7,7,7},
            {3,0},{3,0},{3,0},{3,0});


  cout<<"\n--- Individual classes ---\n";

  {
    FIFO f;
    f.runFIFO(3,{1,2,3,1,4},false,false);
    check("FIFO getters (1 hit, 4 misses, 1 removal)",
          f.getHits()==1 && f.getMisses()==4 && f.getRemovals()==1 && f.getTotal()==5);
    check("FIFO hit ratio is 20%",
          f.getHitRatio()>19.99 && f.getHitRatio()<20.01);
    check("FIFO miss ratio is 80%",
          f.getMissRatio()>79.99 && f.getMissRatio()<80.01);
  }
  {
    LRU l;
    l.runLRU(3,{1,2,3,1,4},false,false);
    check("LRU getters (1 hit, 4 misses, 1 removal)",
          l.getHits()==1 && l.getMisses()==4 && l.getRemovals()==1 && l.getTotal()==5);
  }
  {
    LFU l;
    l.runLFU(3,{1,2,3,1,4},false,false);
    check("LFU getters (1 hit, 4 misses, 1 removal)",
          l.getHits()==1 && l.getMisses()==4 && l.getRemovals()==1 && l.getTotal()==5);
  }
  {
    Optimal o;
    o.runOptimal(3,{1,2,3,1,4},false,false);
    check("Optimal getters (1 hit, 4 misses, 1 removal)",
          o.getHits()==1 && o.getMisses()==4 && o.getRemovals()==1 && o.getTotal()==5);
  }
  {
    FIFO f;
    f.runFIFO(3,{1,2,3,1,4},false,false);
    f.runFIFO(2,{5,5},false,false);
    check("Running an object twice starts fresh",
          f.getHits()==1 && f.getMisses()==1 && f.getRemovals()==0 && f.getTotal()==2);
  }
  {
    LRU l;
    l.runLRU(3,{},false,false);
    check("Empty run gives 0% ratios", l.getHitRatio()==0.0 && l.getMissRatio()==0.0);
  }

  cout<<"\n--- 500 random inputs (compared with simple slow versions) ---\n";

  mt19937 rng(12345);                      
  int badLRU=0, badFIFO=0, badLFU=0, badOpt=0, optNotBest=0, countsWrong=0;
  Simulator sim;
  for(int t=0;t<500;t++){
    int cap=rng()%7;                     
    int n=rng()%31;                        
    int different=rng()%9+1;              
    vector<int> req(n);
    for(int i=0;i<n;i++) req[i]=rng()%(different+1);

    vector<PolicyResult> r=sim.compare(cap,req);
    if(r[0].hits!=simpleLRU(cap,req))     badLRU++;
    if(r[1].hits!=simpleFIFO(cap,req))    badFIFO++;
    if(r[2].hits!=simpleLFU(cap,req))     badLFU++;
    if(r[3].hits!=simpleOptimal(cap,req)) badOpt++;
    if(r[3].hits<r[0].hits || r[3].hits<r[1].hits || r[3].hits<r[2].hits) optNotBest++;
    for(const PolicyResult& p : r)
      if(p.hits+p.misses!=n) countsWrong++;
  }
  check("LRU matches the simple version on all inputs",     badLRU==0);
  check("FIFO matches the simple version on all inputs",    badFIFO==0);
  check("LFU matches the simple version on all inputs",     badLFU==0);
  check("Optimal matches the simple version on all inputs", badOpt==0);
  check("Optimal never has fewer hits than the others",     optNotBest==0);
  check("Hits + misses always equals the number of requests", countsWrong==0);

  cout<<"\n--- Bigger cache ---\n";
  {
  
    vector<int> req={1,2,3,4,1,2,5,1,2,3,4,5};
    bool lruGrows=true, optGrows=true;
    int prevLRU=-1, prevOpt=-1;
    for(int c=0;c<=6;c++){
      vector<PolicyResult> r=sim.compare(c,req);
      if(r[0].hits<prevLRU) lruGrows=false;
      if(r[3].hits<prevOpt) optGrows=false;
      prevLRU=r[0].hits; prevOpt=r[3].hits;
    }
    check("LRU hits never go down when the capacity goes up", lruGrows);
    check("Optimal hits never go down when the capacity goes up", optGrows);
  }

  cout<<"\n===== "<<passed<<" passed, "<<failed<<" failed =====\n";
  if(failed==0) cout<<"All tests passed!\n";
  else cout<<"Some tests failed. Read the FAIL lines above.\n";
  return failed==0 ? 0 : 1;
}
