#include "simulator.h"

bool readInput(int& capacity,vector<int>& requests){
  int n;
  cout<<"Enter cache capacity: ";
  cin>>capacity;
  cout<<"Enter number of requests: ";
  cin>>n;
  if(!cin || capacity<0 || n<0){ cout<<"Invalid input\n"; return false; }
  requests.assign(n,0);
  cout<<"Enter "<<n<<" requests (numbers): ";
  for(int i=0;i<n;i++){
    cin>>requests[i];
    if(!cin || requests[i]<0){ cout<<"Invalid request\n"; return false; }
  }
  return true;
}

int main(){
  Simulator sim;
  int capacity=0;
  vector<int> requests;
  bool haveInput=false;

  while(true){
    cout<<"\n===== Smart Cache Management and Replacement Simulator =====\n";
    cout<<"1. Enter capacity and requests\n";
    cout<<"2. Load sample input (capacity 3)\n";
    cout<<"3. Show LRU step by step\n";
    cout<<"4. Show FIFO step by step\n";
    cout<<"5. Show LFU step by step\n";
    cout<<"6. Show Optimal step by step\n";
    cout<<"7. Compare all policies\n";
    cout<<"8. Compare for capacities 1 to 5\n";
    cout<<"0. Exit\n";
    cout<<"Choice: ";

    int choice;
    if(!(cin>>choice)) break;
    if(choice==0) break;

    if(choice==1){
      haveInput=readInput(capacity,requests);
      if(!cin) break;
    }
    else if(choice==2){
      capacity=3;
      requests={1,2,3,1,4,1,5,1,2,3};
      haveInput=true;
      cout<<"Loaded capacity 3 and requests: 1 2 3 1 4 1 5 1 2 3\n";
    }
    else if(choice>=3 && choice<=8){
      if(!haveInput){ cout<<"Please enter input first (option 1 or 2).\n"; continue; }
      if(choice==3) sim.runSingle("LRU",capacity,requests);
      if(choice==4) sim.runSingle("FIFO",capacity,requests);
      if(choice==5) sim.runSingle("LFU",capacity,requests);
      if(choice==6) sim.runSingle("Optimal",capacity,requests);
      if(choice==7) sim.printComparison(capacity,requests);
      if(choice==8) sim.printCapacityTable(5,requests);
    }
    else{
      cout<<"Wrong choice, try again.\n";
    }
  }
  cout<<"Goodbye!\n";
  return 0;
}
