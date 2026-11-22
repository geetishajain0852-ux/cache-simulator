#include "LFU.h"
int main(){
  int capacity,n;
  cin>>capacity;
  cin>>n;
  LFU cache;                                        
  if(!cin || capacity<0 || n<0){ cout<<"Invalid input\n"; return 0; }
  vector<int> requests(n);
  for(int i=0;i<n;i++){
    cin>>requests[i];
    if(!cin || requests[i]<0){ cout<<"Invalid request\n"; return 0; }
  }
  cache.runLFU(capacity, requests, true);           
  cout<<"--- comparison across capacities ---\n";
  for(int c=1;c<=4;c++) cache.runLFU(c, requests, false);
return 0;
}