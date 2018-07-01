/****************************************************************************
  FileName     [ pdrMgr.cpp ]
  PackageName  [ pdr ]
  Synopsis     [ Define PDR main functions ]
  Author       [ SillyDuck ]
  Copyright    [ Copyright(c) 2016 DVLab, GIEE, NTU, Taiwan ]
 ****************************************************************************/
#define showinfo 0

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdio>
#include <stdlib.h>
#include <cassert>
#include <climits>
#include <cmath>
#include <unistd.h>
#include <queue>
#include <vector>
#include <algorithm>

#include "v3Msg.h"
#include "v3NtkUtil.h"
#include "v3SvrPDRSat.h"
#include "v3NtkHandler.h" // MODIFICATION FOR SoCV BDD
#include "v3Ntk.h"
#include "PDRDef.h"
#include "reader.h"
#include "pdrMgr.h"

using namespace std;

unsigned Cube::_L = 0;

// Compare class for priority_queue
class TCubeCmp {
 public:
  bool operator() (const TCube lhs, const TCube rhs) const {
    return lhs._frame > rhs._frame;
  }
};

// Main verification starts here
void PDRMgr::verifyProperty(const string& name, const V3NetId& monitor) {
  _ntk = new V3Ntk();
  *_ntk = *(v3Handler.getCurHandler()->getNtk());
  SatProofRes pRes;

  V3SvrPDRSat* satSolver = new V3SvrPDRSat(_ntk, false, false);

  // monitor is a V3NetId,
  // Please figure out how V3 present a circuit
  // by V3Ntk and V3NetId in src/ntk/V3Ntk.h and V3Type.h
  satSolver->setMonitor(monitor);

  pRes.setMaxDepth(100);
  pRes.setSatSolver(satSolver);
  double runTime = 0;
  double ctime = (double)clock() / CLOCKS_PER_SEC;

  PDR(monitor, pRes);

  runTime += (((double)clock() / CLOCKS_PER_SEC) - ctime);
  pRes.reportResult(name);
 
  cerr << "runtime: " << runTime << endl;
  delete satSolver; delete _ntk;
  reset();
}

void PDRMgr::reset() {
  _ntk = NULL;
}

void PDRMgr::buildAllNtkVerifyData(const V3NetId& monitor) {
  // Added all circuit constraint to SAT solver here.

  for (uint32_t i = 0; i < _ntk->getLatchSize(); ++i)
     Z->addBoundedVerifyData(_ntk->getLatch(i), 0);
  for (uint32_t i = 0; i < _ntk->getLatchSize(); ++i)
     Z->addBoundedVerifyData(_ntk->getLatch(i), 1);
  Z->addBoundedVerifyData(monitor, 0);
  Z->initValue3Data();
}
//int pf=0;int n=0;

bool PDRMgr::PDR(const V3NetId& monitor, SatProofRes& pRes) {

  // assume no inout
  if (_ntk->getInoutSize()) { assert(0); }
  Z = pRes.getSatSolver();
  L = _ntk->getLatchSize();
  Cube::_L = L;
  //cout<<"latchsize "<<L<<endl;
  F = new vector<vector<Cube*>*>();
  Z->setFrame(F);
  buildAllNtkVerifyData(monitor);
  // this is F_inf
  // which means the cube that will never be reached
  // in any time frame
  // watch out that _frame of cube in this Frame will be INT_MAX
  F->push_back(new vector<Cube*>());

  depth = 0;
  newFrame(); // F[0]

  Z->addInitiateState();
  Z->setMonitor(monitor);

  // PDR Main function
  while (true) {
    // find bad cube, check SAT? (!P & R_k)

    Cube* cube = Z->getBadCube(depth);

    if (debug) {
      if (cube->mapLatch.size()>0) { 
        cerr<<"bad cube in frame:" << depth << endl;
        cube->show();
      } else {
        cerr << "bad cube is NULL\n";
      }
    }
       //cout<<"size "<<(cube->mapLatch).size()<<endl;
       //cube->show();
    if ((cube->mapLatch).size()>0) {        
       
      TCube t(cube, depth);
      // Counter example found
      if (!recursiveBlockCube(t)) {    //cout<<55<<endl;
        pRes.setFired(0);
        if (showinfo) {
          system("clear");
          cerr << "Number of Cubes:" << endl;
          for (unsigned i = 0; i < F->size() - 1; ++i)
            cerr << "Frame "<< i << " : " << (*F)[i]->size() << endl;
          cerr << "Frame INF :" << (*F)[F->size()-1]->size() << endl;
        }
        return true;
      }
    } else {         
      // depth will only be increased here
     

      depth++;
      newFrame();
      // Fixed point
      if (propagateBlockedCubes(pRes)) {      // cout<<66<<endl;
        if (showinfo) {
          system("clear");
          cerr << "Number of Cubes:" << endl;
          for (unsigned i = 0; i < F->size() - 1; ++i)
            cerr << "Frame "<< i << " : " << (*F)[i]->size() << endl;
          cerr << "Frame INF :" << (*F)[F->size()-1]->size() << endl;
        }
        return false;
      }
    }
  }
}

bool PDRMgr::recursiveBlockCube(TCube s0){
  
  if (debug) cerr << "recursiveBlockCube" << endl;
  priority_queue<TCube, vector<TCube>, TCubeCmp> Q;
  Q.push(s0);
  if (debug) {
    cerr << "pushed : " << s0._cube << ", ";
    s0._cube->show();
  }
  while (!Q.empty()) {
     
      
    TCube s = Q.top();                
    Q.pop();
    if (debug) {
      cerr << "poped : " << s._cube << ", ";
      s._cube->show();
    }
    // reach R0, counter example found
    if (s._frame == 0) return false;
    // block s
    if (!isBlocked(s)) {                            
      assert(!(Z->isInitial(s._cube)));
      TCube z = Z->solveRelative(s, 1);
      if (z._frame != -1) {
        // UNSAT, s is blocked
        z = generalize(z);  // UNSAT generalization
        if (debug) {
          cerr << "cube after generalized :";
          z._cube->show();
          cerr << " frame : " << z._frame << endl;
        }
        // push to higher frame
        while (z._frame < (int)(depth - 1)) {      

          // condAssign
          TCube t = Z->solveRelative(next(z), 1);
          if (t._frame != -1) z = t;
          else break;
        }
        addBlockedCube(z);
        if((s._frame < (int)depth) && (z._frame != INT_MAX)) {
          TCube a = next(s);
          Q.push(a);
          if (debug) {
            cerr << "pushed : " << s._cube << ", ";
            s._cube->show();
          }
        }
      } else {
        // SAT, s is not blocked
        // TODO: to generate counterexample trace for unsafe property,
        //       you need to record the SAT pattern here
 
       //static int n=0;

      
        z._frame = s._frame - 1;
    
        Q.push(z);
        if (debug) {
          cerr << "pushed : " << z._cube << ", ";
          z._cube->show();
        }
        Q.push(s); // put it in queue again
        
        if (debug) {
          cerr << "pushed : " << s._cube << ", ";
          s._cube->show();
        }
 /*   COUNTER EXAMPLE TRACE  (NOT WORK)
   Z->getSATAssignmentToCube(z._cube);
       int p=0;
        for (int i=0;i<_ntk->getLatchSize();i++){
            cout<< z._cube->_latchValues[i]._bit;
             //p+=z._cube->_latchValues[i]._bit*pow(2,i) ;
         }
        //cout<<p<<" ";
       int I = _ntk->getInputSize();
  
 bool* ii = new bool[I];
   cout<<n<<": ";
   for (unsigned i = 0; i < I; ++i) {
 	if (Z->existVerifyData(_ntk->getInput(i), 0)) { 
   	Var tv = Z->getVerifyData(_ntk->getInput(i), 0);
    		if (tv){
       			ii[i] = Z->getValue(tv);
                        cout<<ii[i]<<endl;
       			//vec[p]=ii[i];
  		}     
	}  
	else {Msg(MSG_IFO) << 'x';}
        
        ++n;
   }
   */


      }
    }
  }
  return true;
}

bool PDRMgr::isBlocked(TCube s) {
  // check if a cube is already blocked in solver
  // first check if it is subsumes by that frame
  // then check by solver (more time-consuming)
  for (unsigned d = s._frame; d < F->size(); ++d) {
    for (unsigned i = 0; i < (*F)[d]->size(); ++i) {
      if ((*((*F)[d]))[i]->subsumes(s._cube)) {
        if (debug) {
          cerr << "F->size():" << F->size() << endl;
          cerr << "already blocked in frame:" << d << endl;
          (*((*F)[d]))[i]->show();
        }
        return true;
      }
    }
  }
  return Z->isBlocked(s);
}

TCube PDRMgr::generalize(TCube s) {
  // UNSAT generalization
  if (debug) cerr << "UNSAT generalization" << endl;
  map<unsigned, Value3>::iterator iter;
  for (unsigned i = 0; i < L; ++i) {
    Cube* tc = new Cube(s._cube);
    
    iter = tc->mapLatch.find(i);
     
   if (iter!=tc->mapLatch.end()){
    if ( iter->second._dontCare == 1) continue;
    else  iter->second._dontCare = 1;
  
    if (!(Z->isInitial(tc))) {     
      TCube t = Z->solveRelative(TCube(tc, s._frame), 1);
      if (t._frame != -1) s = t;
    }
    else {
       map<unsigned, Value3>::iterator iter2;
       iter2 = s._cube->mapLatch.find(i);
       tc->mapLatch[i]=iter2->second;
 
    }
   }
  }
  return s;
}

bool PDRMgr::propagateBlockedCubes(SatProofRes& pRes) {
  if (debug) cerr << "propagateBlockedCubes" << endl;
  for (unsigned k = 1; k < depth; ++k) {
    for (unsigned i = 0; i < (*F)[k]->size(); ++i) {
      TCube s = Z->solveRelative(TCube((*((*F)[k]))[i], k+1), 2);
      if (s._frame != -1) addBlockedCube(s);
    }
    if ((*F)[k]->size() == 0){
      pRes.setProved(k);
      return true;
    }
  }
  return false;
}

void PDRMgr::newFrame(bool force) {
  if (force || depth >= F->size() - 1) {
    unsigned n = F -> size();
    F->push_back(new vector<Cube*>());
    (*F)[n]->swap(*((*F)[n-1]));
    Z->newActVar();
    assert(Z->_actVars.size() == F->size() - 1); // Frame_inf doesn't need ActVar
    if (debug) {
      cerr << endl;
      cerr << "Newed frame:" << F->size() << endl;
      cerr << "actVar:" << Z->_actVars[Z->_actVars.size() - 1] << endl;
    }
  }
  assert ( depth <= F->size()-2 ) ;
}

void PDRMgr::addBlockedCube(TCube s) {
  assert(s._frame != -1);
  //addBlockedCube (To Frame Structure)
  if (debug) {
    cerr << "addBlockCube in frame : " << s._frame << ", cube is : ";
    s._cube->show();
    cerr << "frame size now is " << F->size() << endl;
  }
  if((unsigned)s._frame == F->size() - 1){
    newFrame(true);
  }
  int l = s._frame;
  int r = F->size()-1;
  unsigned k = (unsigned)(l > r ? r : l);
  for (unsigned d = 1; d <= k; ++d) {
    for (unsigned i = 0; i < (*F)[d]->size(); ) {
      if (s._cube->subsumes((*((*F)[d]))[i])) {
        (*((*F)[d]))[i] = (*F)[d]->back();
        (*F)[d] -> pop_back();
      } else {
        i++;
      }
    }
  }
  (*F)[k]->push_back(s._cube);
  Z->blockCubeInSolver(s);
}

TCube PDRMgr::next(const TCube& s){
  return TCube(s._cube, s._frame + 1);
}
