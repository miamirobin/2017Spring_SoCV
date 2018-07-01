/****************************************************************************
  FileName     [ proveBdd.cpp ]
  PackageName  [ prove ]
  Synopsis     [ For BDD-based verification ]
  Author       [ ]
  Copyright    [ Copyleft(c) 2010 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include "v3NtkUtil.h"
#include "v3Msg.h"
#include "bddMgrV.h"
#include "v3NtkHandler.h" // MODIFICATION FOR SoCV BDD
#include "v3Ntk.h"
#include "bddNodeV.h" // MODIFICATION FOR SoCV BDD
#include <math.h>
#include <vector>
using namespace std;
string DtB(long int a,int level);

void
BddMgrV::buildPInitialState()
{
   // TODO : remember to set _initState
   // Set Initial State to All Zero
  V3Ntk*  hand = v3Handler.getCurHandler()->getNtk();
  BddNodeV a=BddNodeV::_zero;  
  for(int i = 0; i<hand->getLatchSize(); i++) {
  	const V3NetId& id = hand->getLatch(i);
        BddNodeV b=bddMgrV->getBddNodeV(id.id);	
	if (id.cp){a=a|~b;}        
	else      {a=a|b;}
  }                          
   _initState =~a; 

}

void
BddMgrV::buildPTransRelation()
{
   // TODO : remember to set _tr, _tri
   V3Ntk*  hand = v3Handler.getCurHandler()->getNtk();
   V3NtkHandler* handler = v3Handler.getCurHandler();
   BddNodeV a=BddNodeV::_zero;
   for(int i = 0;i< hand->getLatchSize();i++) {
	const V3NetId& id = hand->getLatch(i);		
	const V3NetId& idns=hand->getInputNetId(id,0);
	hand->buildBdd(idns);
        BddNodeV F=bddMgrV->getBddNodeV(idns.id);
        const string ns=handler->getNetNameOrFormedWithId(id)+"_ns" ;
	BddNodeV Y=bddMgrV->getBddNodeV(ns);	
	if(idns.cp){a=a|(Y ^~F);}
        else       {a=a|(Y ^ F);}
    }
    _tri=~a;
    BddNodeV b=_tri;
    int j=1;
    while(j<1+hand->getInputSize()){
        b=b.exist(j);
        j+=1;
     }
    _tr=b;
}

void
BddMgrV::buildPImage( int level )
{
   // TODO : remember to add _reachStates and set _isFixed
   // Note:: _reachStates record the set of reachable states
   
   V3Ntk*  hand = v3Handler.getCurHandler()->getNtk();
   V3NtkHandler* handler = v3Handler.getCurHandler();
    int I=hand->getInputSize();
    int X=hand->getLatchSize();
    int i=1;
    //BddNodeV state= _tr & _initState;
    while(i<=level){
        bool M=false;
        BddNodeV state= _tr & getPReachState();
        //cout<<i<<endl;
        //cout<<getPReachState()<<endl;
        
        int k=1 ;
	while(k<X+I+1){
		state=state.exist(k);
		k+=1;
        }
	if(state.getLevel()!=0){
		state=state.nodeMove(X+I+1,I+1,M);
	}
	if(state==getPReachState()){
            cout<<"Fixed point is reached ( time : " <<_reachStates.size()<<" )" <<endl;
            _isFixed=true;
            break;
        }
        _reachStates.push_back(state); 
        i+=1;
     }                           

   
}

void 
BddMgrV::runPCheckProperty( const string &name, BddNodeV monitor ){
   // TODO : prove the correctness of AG(~monitor)
   BddNodeV ch=monitor&getPReachState();
   V3Ntk*  hand = v3Handler.getCurHandler()->getNtk();
   V3NtkHandler* handler = v3Handler.getCurHandler();
   int I=hand->getInputSize();
   int X=hand->getLatchSize();		
    if(ch==BddNodeV::_zero){
    	if (_isFixed==true){
              cout<<"Monitor "<<name<<" is safe. ";
        }
        else {
              cout<<"Monitor "<<name<<" is safe "<<" up to time " << _reachStates.size()<<"."<<endl;
        }

    }
    else{
        cout<<"Monitor "<<name<<" is violated." <<endl;
	cout<<"Counter Example: " <<endl;
        for (int j=I+1;j<=2*X+I;j++){
		ch=ch.exist(j);
	}
   	vector<string> CE;
        long int bb=pow(2,I);
	for (int i=1;i<bb;i++){
        	string input=DtB(i,I);
        	if(evalCube(ch,input)==1){
                	CE.push_back(input);	
	        }
        }
   	 for (int i=0;i<CE.size();i++){
    		cout<<i<<": "<<CE[i]<<endl;
   	 }
      }
}
string DtB(long int a,int level){
    string bin = "";
    int o = 1;
    for(int i = 0; i < level; i++) {
        if((a&o) <1)      {  bin = "0"+bin;}
        else              { bin = "1"+bin;}
        o<<=1;
    }
    return bin;
}
