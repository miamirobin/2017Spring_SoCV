/****************************************************************************
  FileName     [ satMgr.cpp ]
  PackageName  [ sat ]
  Synopsis     [ Define sat prove package interface ]
  Author       [ ]
  Copyright    [ Copyleft(c) 2010 LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#include <iostream>
#include <cassert>
#include <vector>
#include <queue>
#include "v3Msg.h"
#include "v3NtkUtil.h"
#include "reader.h"
#include "satMgr.h"
  #include "bddNodeV.h" // MODIFICATION FOR SoCV BDD
#include "bddMgrV.h" // MODIFICATION FOR SoCV BDD

extern BddMgrV* bddMgrV; // MODIFICATION FOR SoCV BDD
using namespace std;

void SATMgr::verifyPropertyItp(const string& name, const V3NetId& monitor) {
   // Initialize
   // duplicate the network, so you can modified
   // the ntk for the proving property without
   // destroying the original network
   _ntk = new V3Ntk(); *_ntk = *(v3Handler.getCurHandler()->getNtk());
   SatProofRes pRes;
   SatSolver* satSolver = new SatSolver(_ntk);

   // Prove the monitor here!!
   pRes.setMaxDepth(1000);
   pRes.setSatSolver(satSolver);
   itpUbmc(monitor, pRes);

   pRes.reportResult(name);
   if (pRes.isFired())
      pRes.reportCex(monitor, _ntk);

   delete satSolver; delete _ntk;
   reset();
}

void SATMgr::verifyPropertyBmc(const string& name, const V3NetId& monitor) {
   // Initialize
   // duplicate the network, so you can modified
   // the ntk for the proving property without
   // destroying the original network
   _ntk = new V3Ntk(); *_ntk = *(v3Handler.getCurHandler()->getNtk());
   SatProofRes pRes;
   SatSolver* satSolver = new SatSolver(_ntk);

   // Prove the monitor here!!
   pRes.setMaxDepth(1000);
   pRes.setSatSolver(satSolver);
   indBmc(monitor, pRes);

   pRes.reportResult(name);
   if (pRes.isFired())
      pRes.reportCex(monitor, _ntk);

   delete satSolver; delete _ntk;
   reset();
}

void SATMgr::indBmc(const V3NetId& monitor, SatProofRes& pRes) {
   SatSolver* satSolver = pRes.getSatSolver();
   bind(satSolver);

   uint32_t i = 0; 
   V3NetId I = buildInitState();
   satSolver->addBoundedVerifyData( I, i );
  
   satSolver->assertProperty(I, false, i );
   // Start Bounded Model Checking
   for (uint32_t j = pRes.getMaxDepth(); i < j; ++i) {
      // Add time frame expanded circuit to SAT Solver
      satSolver->addBoundedVerifyData(monitor, i);
      satSolver->assumeRelease();
      satSolver->assumeProperty(monitor, false, i);
      satSolver->simplify();
      // Assumption Solver: If SAT, diproved!
      if(satSolver->assump_solve()) {
         pRes.setFired(i);
         break;
      }
      satSolver->assertProperty(monitor, true, i);
   }
}

void SATMgr::itpUbmc(const V3NetId& monitor, SatProofRes& pRes) {
   SatSolver* satSolver = pRes.getSatSolver();
   bind(satSolver);

   // TODO : finish your own Interpolation-based property checking

   // PART I:
   // Build Initial State
   V3NetId I = buildInitState();  
   
   // PART II:
   //    Take care the first timeframe (i.e. Timeframe 0 ) 
   //    Check if monitor is violated at timeframe 0
   //    Build the whole timeframe 0 and map the var to latch net
   //    Mark the added clauses ( up to now ) to onset
    
   
   
   ClauseId on,off,cumu;
  
   satSolver->addBoundedVerifyData( I, 0 );
   satSolver->assumeRelease();
   satSolver->assumeProperty(I, false, 0 );
    
   // Add time frame expanded circuit to SAT Solver
   satSolver->addBoundedVerifyData(monitor, 0);
   
   satSolver->assumeProperty(monitor, false, 0);
   satSolver->simplify();
    // Assumption Solver: If SAT, diproved!
   if(satSolver->assump_solve()) {
         pRes.setFired(0);
         
         return;
    }
     satSolver->assertProperty(monitor, true, 0);
  
    for (int q=0;q<_ntk->getLatchSize();q++){
     
        
     	const V3NetId& id=_ntk->getLatch(q);
        satSolver->addBoundedVerifyData(id, 1);
        const Var& var=satSolver->getVerifyData(id, 1);
     	 mapVar2Net(var,id );
     }
    
     on=getNumClauses();     
     for(uint32_t z=0; z<on; z++){
		markOnsetClause(z);	
      }    
    
     cumu=getNumClauses();
       uint32_t k=1;
   // PART III:
   //    Start the ITP verification loop
   //    Perform BMC 
   //    SAT  -> cex found
   //    UNSAT-> start inner loop to compute the approx. images
   //    Each time the clauses are added to the solver, 
   //    mark them to onset/offset carefully
   //    ( ex. addedBoundedVerifyData(), assertProperty() are called )
 
     
     for (uint32_t j = pRes.getMaxDepth(); k < j; k++){
        //cout<<k<<endl;
        satSolver->assumeRelease();
        satSolver->assumeProperty(I, false, 0 );
        satSolver->addBoundedVerifyData(monitor, k);
        satSolver->assumeProperty(monitor, false, k);
        satSolver->simplify();        

         if(satSolver->assump_solve()) {
        	 pRes.setFired(k);
        	
                 return;
      }
       
      off=getNumClauses();
     
      for(int z=cumu; z<off; z++){
 		markOffsetClause(z);
      }
           
      cumu=getNumClauses();

     
         
      V3NetId nIdR= I;
 
     
  
 
  while (true){
    
   
   V3NetId nIdS=_ntk->createNet();
   
    satSolver->resizeNtkData(1);
   
    nIdS= getItp();
    
  
  
   satSolver->addBoundedVerifyData( nIdS, 0 );
  
   satSolver->assumeRelease();
   satSolver->assumeProperty(nIdS, false, 0 );    
    
   on=getNumClauses();     
   for(int z=cumu;z<on;z++){
	markOnsetClause(z);
   }
   cumu=getNumClauses();
       
    //satSolver->addBoundedVerifyData(monitor, k);   
    satSolver->assumeProperty(monitor, false, k);
    satSolver->simplify();        

    if(satSolver->assump_solve()) {
     satSolver->assertProperty(monitor, true, k);
        off=getNumClauses();
    for(int z=cumu; z<off; z++){
 	markOffsetClause(z);
     }
         cumu=getNumClauses();
         break;
    }
  
   
  
   
  
     
      V3NetId nId = _ntk->createNet();
      createV3AndGate(_ntk, nId, ~nIdR, ~nIdS );
      V3NetId R_ = ~nId;

      V3NetId c1 =_ntk->createNet();
      V3NetId c2 = _ntk->createNet();
      V3NetId c3 = _ntk->createNet();
      satSolver->assumeRelease();
      createV3AndGate(_ntk, c1,~R_,~nIdR);
      createV3AndGate(_ntk, c2, R_,nIdR);
      createV3AndGate(_ntk, c3,~c1, ~c2);
      satSolver->resizeNtkData(4);
      V3NetId c4=~c3;
      satSolver->addBoundedVerifyData(c4, 0);
      cumu=getNumClauses();
      satSolver->assumeProperty(c4, true, 0);
      satSolver->simplify();
     
      if ( !satSolver->assump_solve()){
               pRes.setProved(k);
               
               return; 
       }     
          
       nIdR=R_;
    }
       

  }
 pRes.setProved(k);

}   

void SATMgr::bind(SatSolver* ptrMinisat) {
   _ptrMinisat = ptrMinisat;
   if (_ptrMinisat->_solver->proof == NULL) {
      Msg(MSG_ERR) << "The Solver has no Proof!! Try Declaring the Solver with proofLog be set!!" << endl;
      exit(0);
   }
}

void SATMgr::reset() {
   _ptrMinisat = NULL;
   _ntk = NULL;
   _varGroup.clear();
   _var2Net.clear();
   _isClauseOn.clear();
   _isClaOnDup.clear();
}

void SATMgr::markOnsetClause(const ClauseId& cid) {
   unsigned cSize = getNumClauses();
   assert(cid < (int)cSize);
   if (_isClauseOn.size() < cSize) {
      _isClauseOn.resize(cSize, false);
   }
   _isClauseOn[cid] = true;
}

void SATMgr::markOffsetClause(const ClauseId& cid) {
   unsigned cSize = getNumClauses();
   assert(cid < (int)cSize);
   if (_isClauseOn.size() < cSize) {
      _isClauseOn.resize(cSize, false);
   }
   _isClauseOn[cid] = false;
}

void SATMgr::mapVar2Net(const Var& var, const V3NetId& net) {
   assert(_var2Net.find(var) == _var2Net.end());
   _var2Net[var] = net;
}

V3NetId SATMgr::getItp() const {
   assert(_ptrMinisat);
   assert(_ptrMinisat->_solver->proof);
   // save proof log
   string proofName = "socv_proof.itp";
   _ptrMinisat->_solver->proof->save(proofName.c_str());

   // bulding ITP
   V3NetId netId = buildItp(proofName);

   // delete proof log
   unlink(proofName.c_str());

   return netId;
}

vector<Clause> SATMgr::getUNSATCore() const {
   assert(_ptrMinisat);
   assert(_ptrMinisat->_solver->proof);

   vector<Clause> unsatCore;
   unsatCore.clear();

   // save proof log
   string proofName = "socv_proof.itp";
   _ptrMinisat->_solver->proof->save(proofName.c_str());

   // generate unsat core
   Reader rdr;
   rdr.open(proofName.c_str());
   retrieveProof(rdr, unsatCore);

   // delete proof log
   unlink(proofName.c_str());

   return unsatCore;
}

void SATMgr::retrieveProof(Reader& rdr, vector<Clause>& unsatCore) const {
   unsigned int tmp, cid, idx, tmp_cid;

   // Clear all
   vector<unsigned int> clausePos; 
   clausePos.clear();
   unsatCore.clear();

   // Generate clausePos
   assert(!rdr.null());
   rdr.seek(0);
   for (unsigned int pos = 0; (tmp = rdr.get64()) != RDR_EOF ;pos = rdr.Current_Pos()) {
      cid = clausePos.size();
      clausePos.push_back(pos);
      if ((tmp & 1) == 0) {  // root clause
         while ((tmp = rdr.get64()) != 0) {}
      } else {              // learnt clause
         idx = 0;
         while ((tmp = rdr.get64()) != 0) { idx = 1; }
         if (idx == 0) clausePos.pop_back(); // Clause Deleted
      }
   }

   // Generate unsatCore
   priority_queue<unsigned int> clause_queue;
   vector<bool> in_queue;
   in_queue.resize(clausePos.size());
   for(unsigned int i = 0; i < in_queue.size(); ++i) in_queue[i] = false;
   in_queue[in_queue.size() - 1] = true;
   clause_queue.push(clausePos.size() - 1); //Push leaf (empty) clause
   while (clause_queue.size() != 0) {
      cid = clause_queue.top();
      clause_queue.pop();

      rdr.seek(clausePos[cid]);

      tmp = rdr.get64();
      if ((tmp & 1) == 0) {
         //root clause
         vec<Lit> lits;
         idx = tmp >> 1;
         lits.push(toLit(idx));
         while (_varGroup[idx >> 1] != COMMON){
            tmp = rdr.get64();
            if (tmp == 0) break;
            idx += tmp;
            lits.push(toLit(idx));
         }
         unsatCore.push_back(Clause(false, lits));
      } else {
         //derived clause
         tmp_cid = cid - (tmp >> 1);
         if (!in_queue[tmp_cid]) {
            in_queue[tmp_cid] = true;
            clause_queue.push(tmp_cid);
         }
         while (1) {
            tmp = rdr.get64();
            if (tmp == 0) break;
            tmp_cid = cid - rdr.get64();
            if (!in_queue[tmp_cid]) {
               in_queue[tmp_cid] = true;
               clause_queue.push(tmp_cid);
            }
         }
      }
   }
}

void SATMgr::retrieveProof(Reader& rdr, vector<unsigned int>& clausePos, vector<ClauseId>& usedClause) const {
   unsigned int tmp, cid, idx, tmp_cid, root_cid;

   // Clear all
   clausePos.clear();
   usedClause.clear();
   _varGroup.clear();
   _varGroup.resize(_ptrMinisat->_solver->nVars(), NONE);
   _isClaOnDup.clear();
   assert((int)_isClauseOn.size() == getNumClauses());

   // Generate clausePos && varGroup
   assert(!rdr.null());
   rdr.seek(0);
   root_cid = 0;
   for (unsigned int pos = 0; (tmp = rdr.get64()) != RDR_EOF ;pos = rdr.Current_Pos()) {
      cid = clausePos.size();
      clausePos.push_back(pos);
      if ((tmp & 1) == 0) {
         //Root Clause
         _isClaOnDup.push_back(_isClauseOn[root_cid]);
         idx = tmp >> 1;
         if (_isClauseOn[root_cid]) {
            if (_varGroup[idx >> 1] == NONE) _varGroup[idx >> 1] = LOCAL_ON;
            else if (_varGroup[idx >> 1] == LOCAL_OFF) _varGroup[idx >> 1] = COMMON;
         } else {
            if (_varGroup[idx >> 1] == NONE) _varGroup[idx >> 1] = LOCAL_OFF;
            else if (_varGroup[idx >> 1] == LOCAL_ON) _varGroup[idx >> 1] = COMMON;
         }
         while(1) {
            tmp = rdr.get64();
            if (tmp == 0) break;
            idx += tmp;
            if (_isClauseOn[root_cid]) {
               if (_varGroup[idx >> 1] == NONE) _varGroup[idx >> 1] = LOCAL_ON;
               else if (_varGroup[idx >> 1] == LOCAL_OFF) _varGroup[idx >> 1] = COMMON;
            } else {
               if (_varGroup[idx >> 1] == NONE) _varGroup[idx >> 1] = LOCAL_OFF;
               else if (_varGroup[idx >> 1] == LOCAL_ON) _varGroup[idx >> 1] = COMMON;
            }
         }
         ++root_cid;
      } else {
         _isClaOnDup.push_back(false);
         idx = 0;
         while (1) {
            tmp = rdr.get64();
            if (tmp == 0) break;
            idx = 1;
            tmp = rdr.get64();
         }
         if (idx == 0) {
            clausePos.pop_back(); // Clause Deleted
            _isClaOnDup.pop_back(); // Clause Deleted
         }
      }
   }

   // Generate usedClause
   priority_queue<unsigned int> clause_queue;
   vector<bool> in_queue;
   in_queue.resize(clausePos.size());
   for(unsigned int i = 0; i < in_queue.size(); ++i ) in_queue[i] = false;
   in_queue[in_queue.size() - 1] = true;
   clause_queue.push(clausePos.size() - 1); //Push root empty clause
   while (clause_queue.size() != 0) {
      cid = clause_queue.top();
      clause_queue.pop();

      rdr.seek(clausePos[cid]);

      tmp = rdr.get64();
      if ((tmp & 1) == 0) continue; //root clause 

      // else, derived clause
      tmp_cid = cid - (tmp >> 1);
      if(!in_queue[ tmp_cid ]) {
         in_queue[tmp_cid] = true;
         clause_queue.push(tmp_cid);
      }
      while (1) {
         tmp = rdr.get64();
         if (tmp == 0) break;
         tmp_cid = cid - rdr.get64();
         if (!in_queue[tmp_cid]) {
            in_queue[tmp_cid] = true;
            clause_queue.push(tmp_cid);
         }
      }
   }
   for (unsigned int i = 0; i < in_queue.size(); ++i) {
      if (in_queue[i]) {
         usedClause.push_back(i);
      }
   }
}

V3NetId SATMgr::buildInitState() const 
{  
     
 // TODO: build initial state
   V3NetId I;
   uint32_t netSize = _ntk->getNetSize();
   //SatSolver* satSolver = new SatSolver(_ntk);
   V3NetId CONST0, CONST1;
   CONST0 = _ntk->getConst(0);
   CONST1 = ~CONST0;
   V3NetId nId, nId1, nId2;
   nId1=CONST1;
   
   const uint32_t n= _ntk->getLatchSize();
   for (int i=0;i<n;i++){
   
   	nId = _ntk->createNet();
        nId2 = _ntk->getLatch(i);
        
   	createV3AndGate(_ntk, nId,nId1, ~nId2);
       
   	nId1=nId;
        
   }
   I=nId1;
   _ptrMinisat->resizeNtkData(_ntk->getNetSize() - netSize);
  return I;
 
}

// build the McMillan Interpolant
V3NetId SATMgr::buildItp(const string& proofName) const {
   Reader rdr;
   // records
   map<ClauseId, V3NetId> claItpLookup;
   vector<unsigned int> clausePos;
   vector<ClauseId> usedClause;
   // ntk
   uint32_t netSize = _ntk->getNetSize();
   // temperate variables
   V3NetId nId, nId1, nId2;
   int i, cid, tmp, idx, tmp_cid;
   // const 1 & const 0
   V3NetId CONST0, CONST1;
   CONST0 = _ntk->getConst(0);
   CONST1 = ~CONST0;

   rdr.open( proofName.c_str() );
   retrieveProof( rdr, clausePos, usedClause );

   for (i = 0; i < (int)usedClause.size() ; i++) {
      cid = usedClause[i];
      rdr.seek(clausePos[cid]);
      tmp = rdr.get64();
      if ((tmp & 1) == 0) {
         // Root Clause
         if (_isClaOnDup[cid]) {
            idx = tmp >> 1;
            while (_varGroup[idx >> 1] != COMMON ) {
               tmp = rdr.get64();
               if (tmp == 0) break;
               idx += tmp;
            }
            if (_varGroup[idx >> 1] == COMMON) {
               assert(_var2Net.find(idx >> 1) != _var2Net.end());
               nId = (_var2Net.find(idx >> 1))->second;
               nId1 = (_var2Net.find(idx >> 1))->second;
               if ((idx & 1) == 1) nId1 = ~nId1;
               if ((idx & 1) == 1) nId = ~nId;
               while (1) {
                  tmp = rdr.get64();
                  if (tmp == 0) break;
                  idx += tmp;
                  if (_varGroup[idx >> 1] == COMMON) {
                     assert(_var2Net.find(idx >> 1) != _var2Net.end());
                     nId2 = (_var2Net.find(idx >> 1))->second;
                     if ((idx & 1) == 1) nId2 = ~nId2;
                     // or
                     nId = ~_ntk->createNet();
                     createV3AndGate(_ntk, nId, ~nId1, ~nId2);
                     nId1 = nId;
                  }
               }
            } else {
               nId = CONST0;
            }
            claItpLookup[cid] = nId;
         } else {
            claItpLookup[cid] = CONST1;
         }
      } else {
         // Derived Clause
         tmp_cid = cid - (tmp >> 1);
         assert(claItpLookup.find(tmp_cid) != claItpLookup.end());
         nId = (claItpLookup.find(tmp_cid))->second;
         nId1 = (claItpLookup.find(tmp_cid))->second;
         while (1) {
            idx = rdr.get64();
            if (idx == 0) break;
            idx--;
            // Var is idx
            tmp_cid = cid - rdr.get64();
            assert(claItpLookup.find(tmp_cid) != claItpLookup.end());
            nId2 = (claItpLookup.find(tmp_cid))->second;
            if (nId1 != nId2) {
               if (_varGroup[idx] == LOCAL_ON) { // Local to A. Build OR Gate.
                  if (nId1 == CONST1 || nId2 == CONST1) {
                     nId = CONST1;
                     nId1 = nId;
                  } else if(nId1 == CONST0) {
                     nId = nId2;
                     nId1 = nId;
                  } else if(nId2 == CONST0) {
                     nId = nId1;
                     nId1 = nId;
                  } else {
                     // or
                     nId = ~_ntk->createNet();
                     createV3AndGate(_ntk, nId, ~nId1, ~nId2);
                     nId1 = nId;
                  }
               } else { // Build AND Gate.
                  if (nId1 == CONST0 || nId2 == CONST0) {
                     nId = CONST0;
                     nId1 = nId;
                  } else if(nId1 == CONST1) {
                     nId = nId2;
                     nId1 = nId;
                  } else if(nId2 == CONST1) {
                     nId = nId1;
                     nId1 = nId;
                  } else {
                     // and
                     nId = _ntk->createNet();
                     createV3AndGate(_ntk, nId, nId1, nId2);
                     nId1 = nId;
                  }
               }
            }
         }
         claItpLookup[cid] = nId;
      }
   }

   cid = usedClause[usedClause.size() - 1];
   nId = claItpLookup[cid];

   _ptrMinisat->resizeNtkData(_ntk->getNetSize() - netSize); // resize Solver data to ntk size

   return nId;
}

void SatProofRes::reportResult(const string& name) const {
   // Report Verification Result
   Msg(MSG_IFO) << endl;
   if (isProved()) {
      Msg(MSG_IFO) << "Monitor \"" << name << "\" is safe." << endl;
   } else if (isFired()) {
      Msg(MSG_IFO) << "Monitor \"" << name << "\" is violated." << endl;
   } else {
      Msg(MSG_IFO) << "UNDECIDED at depth = " << _maxDepth << endl;
   }
}

void SatProofRes::reportCex(const V3NetId& monitor, const V3Ntk* const ntk) const {
   assert (_satSolver != 0);

   // Output Pattern Value (PI + PIO)
   V3BitVecX dataValue;
   for (uint32_t i = 0; i <= _fired; ++i) {
      Msg(MSG_IFO) << i << ": ";
      for (int j = ntk->getInoutSize()-1; j >= 0; --j) {
         if (_satSolver->existVerifyData(ntk->getInout(j), i)) {
            dataValue = _satSolver->getDataValue(ntk->getInout(j), i);
            assert (dataValue.size() == ntk->getNetWidth(ntk->getInout(j)));
            Msg(MSG_IFO) << dataValue[0];
         } else {
            Msg(MSG_IFO) << 'x';
         }
      }
      for (int j = ntk->getInputSize()-1; j >= 0; --j) {
         if (_satSolver->existVerifyData(ntk->getInput(j), i)) {
            dataValue = _satSolver->getDataValue(ntk->getInput(j), i);
            assert (dataValue.size() == ntk->getNetWidth(ntk->getInput(j)));
            Msg(MSG_IFO) << dataValue[0];
         } else {
            Msg(MSG_IFO) << 'x';
         }
      }
      Msg(MSG_IFO) << endl;
      assert (_satSolver->existVerifyData(monitor, i));
   }
}
