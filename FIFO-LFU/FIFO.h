#pragma once
#include<iostream>
#include<unordered_map>
#include<vector>
#include<iomanip>
using namespace std;
class FIFO{
  struct Node{
    int key;
    int val;
    Node* next;
    Node* prev;
    Node(int key1,int val1,Node* next1,Node* prev1){
      key=key1;
      val=val1;
      next=next1;
      prev=prev1;
    }
  };

  unordered_map<int,Node*> mpp;   
  Node* head=nullptr;             
  Node* tail=nullptr;             
  int Capacity=0;
  int hits=0, misses=0, removals=0, total=0;
  bool removedAny=false;         
  int lastRemoved=0;             

  void initialize(int capacity){
    Capacity=(capacity<0)?0:capacity;
    head=new Node(-1,-1,nullptr,nullptr);
    tail=new Node(-1,-1,nullptr,head);
    head->next=tail;
  }

  void cleanup(){
    while(head!=nullptr){
      Node* temp=head;
      head=head->next;
      delete temp;
    }
    tail=nullptr;
    mpp.clear();
  }
  void deletion(Node* node){
    Node* after=node->next;
    Node* before=node->prev;
    node->next=nullptr;
    node->prev=nullptr;
    after->prev=before;
    before->next=after;
  }

 
  void insertion(Node* node){
    node->next=head->next;
    node->prev=head;
    head->next->prev=node;
    head->next=node;
  }

  void put(int key,int val){
    if(Capacity==0) return;
    if(mpp.find(key)!=mpp.end()){
      mpp[key]->val=val;
      return;
    }
    if((int)mpp.size()==Capacity){
      Node* oldest=tail->prev;    
      lastRemoved=oldest->key;
      removedAny=true;
      removals++;
      mpp.erase(oldest->key);
      deletion(oldest);
      delete oldest;
    }
    Node* x=new Node(key,val,nullptr,nullptr);
    mpp[key]=x;
    insertion(x);
  }
  bool get(int key){
    removedAny=false;
    if(mpp.find(key)==mpp.end()){
      misses++;
      return false;
    }
    hits++;
    return true;
  }
  void display(){
    cout<<"cache: ";
    for(Node* p=tail->prev;p!=head;p=p->prev) cout<<p->key<<" ";
    cout<<"\n";
  }

public:
  ~FIFO(){ cleanup(); }

  void runFIFO(int capacity,const vector<int>& requests,bool show,bool printSummary=true){
    cleanup();
    hits=0; misses=0; removals=0; removedAny=false;
    total=(int)requests.size();
    initialize(capacity);
    for(int x : requests){
      if(get(x)){ if(show) cout<<x<<" HIT  "; }
      else { put(x,x); if(show) cout<<x<<" MISS "; }
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
    cleanup();
  }
  int getHits() const { return hits; }
  int getMisses() const { return misses; }
  int getRemovals() const { return removals; }
  int getTotal() const { return total; }
  double getHitRatio() const { return total ? 100.0*hits/total : 0.0; }
  double getMissRatio() const { return total ? 100.0*misses/total : 0.0; }
};
