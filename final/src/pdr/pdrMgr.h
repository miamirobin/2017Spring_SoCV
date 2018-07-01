/****************************************************************************
  FileName     [ pdrMgr.cpp ]
  PackageName  [ pdr ]
  Synopsis     [ Define PDR main functions ]
  Author       [ SillyDuck ]
  Copyright    [ Copyright(c) 2016 DVLab, GIEE, NTU, Taiwan ]
 ****************************************************************************/

#ifndef PDR_MGR_H_
#define PDR_MGR_H_

#include <unistd.h>
#include <cassert>
#include <vector>
#include <map>
#include "v3SvrPDRSat.h"

//
// SoCV Final

// Things you can try:
//   Memory and deletion ^.<
//   Data structure cube and TCube and Frame
//   Generalize order
//   if (_pdrActCount too much) recycleSolver();

class V3SvrPDRSat;
class V3NetId;

class SatProofRes {
  public:
    SatProofRes(V3SvrPDRSat* s = 0):
      _proved(V3NtkUD), _fired(V3NtkUD), _maxDepth(V3NtkUD), _satSolver(s) {}

    void setProved(uint32_t i) { _proved = i; }
    void setFired(uint32_t i) { _fired = i; }

    bool isProved() const { return (_proved != V3NtkUD); }
    bool isFired() const { return (_fired != V3NtkUD); }

    void setMaxDepth(uint32_t d) { _maxDepth = d; }
    uint32_t getMaxDepth() const { return _maxDepth; }

    void setSatSolver(V3SvrPDRSat* s) { _satSolver = s; }
    V3SvrPDRSat* getSatSolver() const { return _satSolver; }

    void reportResult(const string&) const;
    void reportCex(const V3NetId&, const V3Ntk* const) const;

  private:
    uint32_t      _proved;
    uint32_t      _fired;
    uint32_t      _maxDepth;  // maximum proof depth
    V3SvrPDRSat*  _satSolver;
};

class PDRMgr {
  public:
    PDRMgr(): _ntk(NULL) { reset(); }
    ~PDRMgr() { reset(); }

    // entry point for SoCV SAT property checking
    void   verifyProperty(const string& name, const V3NetId& monitor); 
    void   reset();

    bool   PDR(const V3NetId& monitor, SatProofRes& pRes);
    bool   recursiveBlockCube(TCube s0);
    bool   isBlocked(TCube s);
    TCube  generalize(TCube s);
    bool   propagateBlockedCubes(SatProofRes& pRes);
    void   newFrame(bool force = false);
    bool   condAssign(TCube& s, TCube t);
    void   addBlockedCube(TCube s);
    TCube  next(const TCube& s);
    void   buildAllNtkVerifyData(const V3NetId& monitor);
  private:
    // PDR implement by SillyDuck in form of Original PDR paper :
    // "Efficient implementation of property directed reachability." FMCAD 2011
    // by Een, Niklas, Alan Mishchenko, and Robert Brayton.
    //
    // READ THIS PAPER AGAIN AND AGAIN!!!
    //
    V3Ntk*                    _ntk;
    vector<vector<Cube*>*>*   F;         // be careful of the type of this data member
    unsigned                  L;         // latch size
    V3SvrPDRSat*              Z;         // PDR Sat interface
    unsigned                  depth;
};

#endif

