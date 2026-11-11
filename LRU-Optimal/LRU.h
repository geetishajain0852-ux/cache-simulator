#pragma once
#include<iostream>
#include<unordered_map>
#include<vector>
#include<iomanip>
using namespace std;
class LRU{
  struct Node{
int key;
int val;
Node*next;
Node*prev;
Node(int key1,int val1,Node*next1,Node*prev1){
key=key1;
val=val1;
next=next1;
prev=prev1;
}
};
unordered_map<int,Node*>mpp;
Node*head;
Node*tail;
int Capacity;
int hits=0, misses=0, removals=0;
int lastRemoved=-1; 
void initialize(int capacity)
{
 Capacity=capacity;
head=new Node(-1,-1,nullptr,nullptr);
tail=new Node(-1,-1,nullptr,head);
head->next=tail;
}
void cleanup(){
while(head != nullptr){
    Node* temp = head;
    head = head->next;
    delete temp;
}
mpp.clear();
}
Node*deletion(Node*node){
  Node*y=node;
  Node*temp=y->next;
  Node*z=y->prev;
  y->next=nullptr;
  y->prev=nullptr;
  temp->prev=z;
  z->next=temp;
  return y;
}
Node*insertion(Node*node){
node->next=head->next;
  node->prev=head;
head->next->prev=node;
head->next=node;
return node;
}
Node*put(int key,int val){
  if (Capacity == 0){
   return nullptr;
  }
  if (mpp.find(key)!=mpp.end()){
    Node*y=mpp[key];
    y->val=val;
    deletion(y);
    insertion(y);
    return y;
  }
  if ((int)mpp.size()==Capacity)
  {
    Node*y=tail->prev;
    lastRemoved=y->key;    
  removals++;
    mpp.erase(y->key);
    deletion(y);
    delete y;
  }
Node*x=new Node(key,val,nullptr,head);
mpp[key]=x;
insertion(x);
return x;
}
int get(int key){
  lastRemoved=-1; 
  if (mpp.find(key)==mpp.end())
  {
    misses++; 
    return -1;
  }
  
Node*y=mpp[key];
deletion(y);
insertion(y);
hits++;
return y->val;
}
void display(){
  cout<<"cache: ";
  for(Node*p=head->next;p!=tail;p=p->next) cout<<p->key<<" ";
  cout<<"\n";
}
public:
void runLRU(int capacity, const vector<int>& requests, bool show){
  hits=0; misses=0; removals=0; lastRemoved=-1;
  initialize(capacity);
  for(int x : requests){
    if(get(x)!=-1){ if(show) cout<<x<<" HIT  "; }
    else { put(x,x); if(show) cout<<x<<" MISS "; }
    if(show){
      cout<<"removed: ";
      if(lastRemoved==-1) cout<<"-"; else cout<<lastRemoved;
      cout<<" | ";
      display();
    }
  }
  int n = requests.size();
  cout<<fixed<<setprecision(2);
  cout<<"Capacity: "<<capacity<<" | Hits: "<<hits<<" Misses: "<<misses
      <<" Hit ratio: "<<(n?100.0*hits/n:0)<<"% Miss ratio: "<<(n?100.0*misses/n:0)
      <<"% Replacements: "<<removals<<"\n";
  cleanup();
}
};
