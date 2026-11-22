#pragma once
#include<iostream>
#include<unordered_map>
#include<vector>
#include<iomanip>
using namespace std;

class LFU{
  struct Node{
    int key;
    int val;
    int freq;     
    Node* next;
    Node* prev;
    Node(int key1,int val1,Node* next1,Node* prev1){
      key=key1;
      val=val1;
      next=next1;
      prev=prev1;
      freq=1;
    }
  };

  struct List{
    Node* head;
    Node* tail;
    int size;
    List(){
      head=new Node(-1,-1,nullptr,nullptr);
      tail=new Node(-1,-1,nullptr,head);
      head->next=tail;
      size=0;
    }
    ~List(){
      Node* temp=head->next;
      while(temp!=tail){
        Node* nextNode=temp->next;
        delete temp;
        temp=nextNode;
      }
      delete head;
      delete tail;
    }
    void deletion(Node* node){
      Node* after=node->next;
      Node* before=node->prev;
      node->next=nullptr;
      node->prev=nullptr;
      after->prev=before;
      before->next=after;
      size--;
    }
    void insertion(Node* node){
      node->next=head->next;
      node->prev=head;
      head->next->prev=node;
      head->next=node;
      size++;
    }
  };

  int Capacity=0;
  int minfreq=0;       
  int maxfreq=0;       
  int hits=0, misses=0, removals=0, total=0;
  bool removedAny=false;   
  int lastRemoved=0;       
  unordered_map<int,Node*> keyNode;
  unordered_map<int,List*> freqList;

  void cleanup(){
    for(auto& it : freqList){
      delete it.second;
    }
    freqList.clear();
    keyNode.clear();
  }

  void updateFreq(Node* y){
    freqList[y->freq]->deletion(y);
    if(y->freq==minfreq && freqList[minfreq]->size==0){
      minfreq++;
    }
    if(freqList.find(y->freq+1)==freqList.end()){
      freqList[y->freq+1]=new List();
    }
    y->freq++;
    if(y->freq>maxfreq) maxfreq=y->freq;
    freqList[y->freq]->insertion(y);
  }
  void put(int key,int val){
    if(Capacity==0) return;
    if(keyNode.find(key)!=keyNode.end()){
      Node* y=keyNode[key];
      y->val=val;
      updateFreq(y);
      return;
    }
    if((int)keyNode.size()==Capacity){
      List* list=freqList[minfreq];
      Node* victim=list->tail->prev;   
      lastRemoved=victim->key;
      removedAny=true;
      removals++;
      keyNode.erase(victim->key);
      list->deletion(victim);
      delete victim;
    }
    minfreq=1;                       
    if(maxfreq<1) maxfreq=1;
    Node* x=new Node(key,val,nullptr,nullptr);
    keyNode[key]=x;
    if(freqList.find(1)==freqList.end()){
      freqList[1]=new List();
    }
    freqList[1]->insertion(x);
  }
  bool get(int key){
    removedAny=false;
    if(keyNode.find(key)==keyNode.end()){
      misses++;
      return false;
    }
    updateFreq(keyNode[key]);
    hits++;
    return true;
  }
  void display(){
    cout<<"cache: ";
    for(int f=1;f<=maxfreq;f++){
      if(freqList.find(f)==freqList.end()) continue;
      List* l=freqList[f];
      for(Node* p=l->tail->prev;p!=l->head;p=p->prev)
        cout<<p->key<<"("<<f<<") ";
    }
    cout<<"\n";
  }

public:
  LFU(){}
  ~LFU(){ cleanup(); }

  void runLFU(int capacity,const vector<int>& requests,bool show,bool printSummary=true){
    cleanup();
    hits=0; misses=0; removals=0; removedAny=false;
    total=(int)requests.size();
    Capacity=(capacity<0)?0:capacity;
    minfreq=0; maxfreq=0;
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
