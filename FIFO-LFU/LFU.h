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
Node*next;
Node*prev;
Node(int key1,int val1,Node*next1,Node*prev1){
key=key1;
val=val1;
next=next1;
prev=prev1;
freq=1;
}
};
struct List{
Node*head;
Node*tail;
int size;
List()
{
head=new Node(-1,-1,nullptr,nullptr);
tail=new Node(-1,-1,nullptr,head);
head->next=tail;
size=0;
}
~List(){
  Node*temp=head->next;
  while (temp!=tail)
  {
    Node*nextNode=temp->next;
    delete temp;
    temp=nextNode;
  }
  
  delete head;
delete tail;
}
void deletion(Node*node){
  Node*y=node;
  Node*temp=y->next;
  Node*z=y->prev;
  y->next=nullptr;
  y->prev=nullptr;
  temp->prev=z;
  z->next=temp;
  size--;
}
void insertion(Node*node){
node->next=head->next;
  node->prev=head;
head->next->prev=node;
head->next=node;
size++;
}
};
  int maxfreq;
int hits=0, misses=0, removals=0;
int lastRemoved=-1;
int Capacity;
int minfreq;
unordered_map<int,Node*>keyNode;
unordered_map<int,List*>freqList;
void cleanup(){
for(auto it:freqList){
    delete it.second;
  }
  freqList.clear();
  keyNode.clear();
}
void updateFreq(Node*y){
freqList[y->freq]->deletion(y);
if (y->freq==minfreq && freqList[minfreq]->size==0)
{
  minfreq++;
}
if (freqList.find(y->freq+1)==freqList.end())
{
  List* newfreqList=new List();
  freqList[y->freq+1]=newfreqList;
}
y->freq++;
if(y->freq>maxfreq) maxfreq=y->freq;
freqList[y->freq]->insertion(y);
}
void put(int key,int val){
if (Capacity == 0){
   return ;
  }
  if (keyNode.find(key)!=keyNode.end()){
    Node*y=keyNode[key];
    y->val=val;
    updateFreq(y);
    return ;
  }
  if ((int)keyNode.size()==Capacity)
  {
    List*list=freqList[minfreq];
    Node*temp=list->tail->prev;
    lastRemoved=temp->key;     
    removals++;
    keyNode.erase(temp->key);
    list->deletion(temp);
    delete temp;
  }
minfreq=1;
if(maxfreq<1) maxfreq=1;
Node*x=new Node{key,val,nullptr,nullptr};
keyNode[key]=x;
if (freqList.find(minfreq)==freqList.end())
{
  List* newfreqList=new List();
  freqList[minfreq]=newfreqList;
}
freqList[minfreq]->insertion(x);
}
int get(int key){
  lastRemoved=-1;
 if (keyNode.find(key)==keyNode.end())
  {
    misses++; 
    return -1;
  }
  
Node*y=keyNode[key];
updateFreq(y);
hits++; 
return y->val;
}
void display(){
  cout<<"cache: ";
  for(int f=1;f<=maxfreq;f++){
    if(freqList.find(f)==freqList.end()) continue;
    List*l=freqList[f];
    for(Node*p=l->tail->prev;p!=l->head;p=p->prev)
      cout<<p->key<<"("<<f<<") ";
  }
  cout<<"\n";
}
public:
LFU(){
Capacity=0;
minfreq=0;
maxfreq=0;
}
~LFU(){
  cleanup();
}
void runLFU(int capacity, const vector<int>& requests, bool show){
  cleanup();
  hits=0; misses=0; removals=0; lastRemoved=-1;
  Capacity=capacity; minfreq=0; maxfreq=0;
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

