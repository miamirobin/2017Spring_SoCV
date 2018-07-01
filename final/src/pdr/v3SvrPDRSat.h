/****************************************************************************
  FileName     [ v3SvrPDRSat.h ]
  PackageName  [ v3/src/pdr ]
  Synopsis     [ Define PDR sat sovler interface ]
  Author       [ Cheng-Yin Wu, SillyDuck ]
  Copyright    [ Copyright(c) 2016 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#define debug 0

#ifndef V3_SVR_MSAT_H
#define V3_SVR_MSAT_H

#include <iostream>
#include "SolverV.h"
#include "PDRDef.h"
#include "reader.h"
#include "v3Msg.h"
#include "v3NtkUtil.h"
#include "v3NtkHandler.h" // MODIFICATION FOR SoCV
#include "v3Ntk.h"

typedef V3Vec<size_t>::Vec      V3SvrDataVec;
typedef V3Vec<Var>::Vec         V3SvrMVarData;
typedef V3Vec<Lit>::Vec         V3SvrMLitData;

class PDRMgr; // MODIFICATION FOR SoCV

class V3SvrPDRSat
{
   friend class PDRMgr; // MODIFICATION FOR SoCV

   public : 
      // Constructor and Destructor
      V3SvrPDRSat(V3Ntk*, const bool& = false, const bool& = false); // MODIFICATION FOR SoCV
      ~V3SvrPDRSat();
      // Basic Operation Functions
      void reset();
      void assumeInit();
      void assertInit();
      void initRelease();
      void assumeRelease();
      void assumeProperty(const size_t&, const bool& = false);
      void assertProperty(const size_t&, const bool& = false);
      void assumeProperty(const V3NetId&, const bool&, const uint32_t&);
      void assertProperty(const V3NetId&, const bool&, const uint32_t&);
      const bool simplify();
      const bool solve();
      const bool assump_solve();
      // Manipulation Helper Functions
      void setTargetValue(const V3NetId&, const V3BitVecX&, const uint32_t&, V3SvrDataVec&);
      void assertImplyUnion(const V3SvrDataVec&);
      const size_t setTargetValue(const V3NetId&, const V3BitVecX&, const uint32_t&, const size_t&);
      const size_t setImplyUnion(const V3SvrDataVec&);
      const size_t setImplyIntersection(const V3SvrDataVec&);
      const size_t setImplyInit();
      const V3BitVecX getDataValue(const V3NetId&, const uint32_t&) const;
      const bool getDataValue(const size_t&) const;
      void getDataConflict(V3SvrDataVec&) const;
      const size_t getFormula(const V3NetId&, const uint32_t&);
      const size_t getFormula(const V3NetId&, const uint32_t&, const uint32_t&);
      // Variable Interface Functions
      inline const size_t reserveFormula() { return getPosVar(newVar(1)); }
      inline const bool isNegFormula(const size_t& v) const { return (v & 1ul); }
      inline const size_t getNegFormula(const size_t& v) const { return (v ^ 1ul); }
      // Print Data Functions
      void printInfo() const;
      // Gate Formula to Solver Functions
      void add_FALSE_Formula(const V3NetId&, const uint32_t&);
      void add_PI_Formula(const V3NetId&, const uint32_t&);
      void add_FF_Formula(const V3NetId&, const uint32_t&);
      void add_AND_Formula(const V3NetId&, const uint32_t&);
      inline const bool validNetId(const V3NetId& id) const { return _ntk->getNetSize() > id.id; }
      inline const uint32_t totalSolves() const { return _solves; }
      inline const double totalTime() const { return _runTime; }

      // Network to Solver Functions
      void addBoundedVerifyData(const V3NetId&, const uint32_t&);
      const bool existVerifyData(const V3NetId&, const uint32_t&);
      // MODIFICATION FOR SoCV
      void resizeNtkData(const uint32_t&);
      void addVerifyData(const V3NetId&, const uint32_t&);
      void addBoundedVerifyDataRecursively(const V3NetId&, uint32_t&);
      // MiniSat Functions
      const Var newVar(const uint32_t&);
      const Var getVerifyData(const V3NetId&, const uint32_t&) const;
      // Helper Functions : Transformation Between Internal and External Representations
      inline const Var getOriVar(const size_t& v) const { return (Var)(v >> 1ul); }
      inline const size_t getPosVar(const Var& v) const { return (((size_t)v) << 1ul); }
      inline const size_t getNegVar(const Var& v) const { return ((getPosVar(v)) | 1ul); }

      /*------------------------------------------------*\
          Functions above are almost not relavant to your SoCV final
          But these functions are relavant...
      \*------------------------------------------------*/
      void setFrame(vector< vector<Cube*>* >* f);
      void setMonitor(const V3NetId& m);
      void addInitiateState();

      void getSATAssignmentToCube(Cube* cube);
      bool* ternarySimInit(Cube* c);
      Cube* ternarySimulation(Cube* c, bool b, bool* input);

      void newActVar();
      void initValue3Data();
      void dfs(V3NetVec& orderedNets);
      void v3SimOneGate(V3NetId id);
      int getValue(Var v) const;

      Cube* getBadCube(uint depth);
      bool isBlocked(TCube c);
      bool isInitial(Cube* c);
      void blockCubeInSolver(TCube s);
      Cube* UNSATGeneralizationWithUNSATCore(Cube* c, vector<Lit>& Lit_vec_origin);
      Var addNotSToSolver(Cube* c);
      void addNextStateSToSolver(Cube* c, vector<Lit>& Lit_vec_origin);
      void assertCubeUNSAT(Cube*c, uint d);
      TCube solveRelative(TCube s, size_t param);

   // making them public is unhealty
   // private :
      // Data Members
      V3Ntk*                    _ntk;       // Network Under Verification
      uint32_t                  _solves;    // Number of Solve Called
      double                    _runTime;   // Total Runtime in Solving
      const bool                _freeBound; // Set FF Bounds Free

      SolverV*                  _Solver;    // Pointer to a Minisat solver
      Var                       _curVar;    // Latest Fresh Variable
      vec<Lit>                  _assump;    // Assumption List for assumption solve

      V3SvrMLitData             _init;      // Initial state Var storage
      V3SvrMVarData*            _ntkData;   // Mapping between V3NetId and Solver Data (Vars)
      vector<vector<Cube*>*>*   _F;         // PDR Frames
      const size_t              _L;         // Latch size
      const size_t              _I;         // Input size
      vector<Var>               _actVars;   // Activation Vars
      V3NetId                   _monitor;   // The Bad Output
      V3Vec<Value3>::Vec        _Value3List;// Mapping between V3NetId and Value3, used for simulation
};

#endif

