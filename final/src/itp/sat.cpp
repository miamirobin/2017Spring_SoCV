/****************************************************************************
  FileName     [ sat.cpp ]
  PackageName  [ sat ]
  Synopsis     [ Define miniSat solver interface functions ]
  Author       [ Chung-Yang (Ric) Huang, Cheng-Yin Wu ]
  Copyright    [ Copyleft(c) 2010-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef SAT_C
#define SAT_C

#include "sat.h"
#include <cmath>

SatSolver::SatSolver(const V3Ntk* const ntk ): _ntk(ntk)
{
    _solver = new SolverV();
    _solver->proof = new Proof(); 
    _assump.clear();
    _curVar = 0; _solver->newVar(); ++_curVar;
    _ntkData = new vector<Var> [ntk->getNetSize()];
    for(uint32_t i = 0; i < ntk->getNetSize(); ++i ) _ntkData[i].clear();
}

SatSolver::~SatSolver() 
{
    delete _solver;  
    assumeRelease();
    for(uint32_t i = 0; i < _ntk->getNetSize(); ++i ) _ntkData[i].clear();
    delete [] _ntkData;
}

void SatSolver::reset()
{
    delete _solver; 
    _solver = new SolverV();
    _solver->proof = new Proof(); 
    _assump.clear();
    _curVar = 0; _solver->newVar(); ++_curVar;
    _ntkData = new vector<Var> [_ntk->getNetSize()];
    for(uint32_t i = 0; i < _ntk->getNetSize(); ++i ) _ntkData[i].clear();
}

void SatSolver::assumeRelease() { _assump.clear(); }

void SatSolver::assumeProperty(const size_t& var, const bool& invert)
{
    _assump.push(mkLit(getOriVar(var), invert ^ isNegFormula(var)));
}

void SatSolver::assertProperty(const size_t& var, const bool& invert)
{
    _solver->addUnit(mkLit(getOriVar(var), invert ^ isNegFormula(var)));
}

void SatSolver::assumeProperty(const V3NetId& id, const bool& invert, const uint32_t& depth)
{
    assert(validNetId(id)); assert( 1 == _ntk->getNetWidth(id) );
    const Var var = getVerifyData(id, depth); assert(var);
    _assump.push(mkLit(var, invert ^ isV3NetInverted(id)));
}

void SatSolver::assertProperty(const V3NetId& id, const bool& invert, const uint32_t& depth)
{
    assert(validNetId(id)); assert( 1 == _ntk->getNetWidth(id) );
    const Var var = getVerifyData(id, depth); assert(var);
    _solver->addUnit(mkLit(var, invert ^ isV3NetInverted(id)));
}

const bool SatSolver::simplify() { return _solver->simplifyDB(); }

const bool SatSolver::solve()
{
    _solver->solve();
    return _solver->okay();
}

const bool SatSolver::assump_solve()
{
    bool result = _solver->solve(_assump);
    return result;
}

const V3BitVecX SatSolver::getDataValue(const V3NetId& id, const uint32_t& depth) const 
{
    Var var = getVerifyData(id, depth); assert(var);
    uint32_t i, width = _ntk->getNetWidth(id);
    V3BitVecX value(width);
    if(isV3NetInverted(id)) {
        for(i = 0; i < width; ++i)
            if(l_True == _solver->model[var+i]) value.set0(i);
            else value.set1(i);
    }
    else {
        for(i = 0; i < width; ++i)
            if(l_True == _solver->model[var+i]) value.set1(i);
            else value.set0(i);
    }
    return value;
}

const bool SatSolver::getDataValue(const size_t& var) const 
{
    return (isNegFormula(var)) ^ (l_True == _solver->model[getOriVar(var)] );
}

const size_t SatSolver::getFormula(const V3NetId& id, const uint32_t& depth)
{
    Var var = getVerifyData(id, depth); assert(var);
    assert(!isNegFormula(getPosVar(var)));
    return (isV3NetInverted(id) ? getNegVar(var) : getPosVar(var));
}

void SatSolver::resizeNtkData(const uint32_t& num)
{
    vector<Var>* tmp = new vector<Var>[_ntk->getNetSize()];
    for(uint32_t i = 0, j = (_ntk->getNetSize()-num); i < j; ++i)
        tmp[i] = _ntkData[i];
    delete [] _ntkData;
    _ntkData = tmp;
}

const Var SatSolver::newVar() {
    Var cur_var = _curVar;
    _solver->newVar();
    _curVar++; 
    return cur_var;
}

const Var SatSolver::getVerifyData(const V3NetId& id, const uint32_t& depth) const
{
    assert(validNetId(id));
    if ( depth >= _ntkData[getV3NetIndex(id)].size() ) return 0;
    else return _ntkData[getV3NetIndex(id)][depth];
}

void SatSolver::add_FALSE_Formula(const V3NetId& out, const uint32_t& depth)
{
    const uint32_t index = getV3NetIndex(out); 
    assert( depth == _ntkData[index].size());
    _ntkData[index].push_back(newVar()); 
    _solver->addUnit(mkLit(_ntkData[index].back(), true));
}

void SatSolver::add_PI_Formula(const V3NetId& out, const uint32_t& depth)
{
    const uint32_t index = getV3NetIndex(out); 
    assert( depth == _ntkData[index].size());
    _ntkData[index].push_back(newVar()); 
}

void SatSolver::add_FF_Formula(const V3NetId& out, const uint32_t& depth) 
{
    const uint32_t index = getV3NetIndex(out);
    assert( depth == _ntkData[index].size());

    if(depth) {
        // Build FF I/O Relation
        const V3NetId in1 = _ntk->getInputNetId(out,0); assert(validNetId(in1));
        const Var var1 = getVerifyData(in1, depth-1); assert(var1);

        if(isV3NetInverted(in1)) {
            // a <-> b
            _ntkData[index].push_back(newVar());
            Lit a = mkLit(_ntkData[index].back());
            Lit b = mkLit(var1,true);
            vec<Lit> lits; lits.clear();
            lits.push(~a); lits.push( b); _solver->addClause(lits); lits.clear();
            lits.push( a); lits.push(~b); _solver->addClause(lits); lits.clear();
        }
        else _ntkData[index].push_back(var1);
    }
    else {  // Timeframe 0
        _ntkData[index].push_back(newVar());
    }
}

void SatSolver::add_AND_Formula(const V3NetId& out, const uint32_t& depth)
{
    const uint32_t index = getV3NetIndex(out); 
    assert( depth == _ntkData[index].size());
    _ntkData[index].push_back(newVar()); 

    const Var& var = _ntkData[index].back();
    // Build AND I/O Relation
    const V3NetId in1 = _ntk->getInputNetId(out,0); assert(validNetId(in1));
    const V3NetId in2 = _ntk->getInputNetId(out,1); assert(validNetId(in2));
    const Var var1 = getVerifyData(in1,depth); assert(var1);
    const Var var2 = getVerifyData(in2,depth); assert(var2);

    Lit y = mkLit(var);
    Lit a = mkLit(var1, isV3NetInverted(in1));
    Lit b = mkLit(var2, isV3NetInverted(in2));

    vec<Lit> lits; lits.clear();
    lits.push(a); lits.push(~y); _solver->addClause(lits); lits.clear();
    lits.push(b); lits.push(~y); _solver->addClause(lits); lits.clear();
    lits.push(~a); lits.push(~b); lits.push(y); _solver->addClause(lits); lits.clear();
}

void SatSolver::addBoundedVerifyData(const V3NetId& id, uint32_t& depth)
{
    if( existVerifyData( id, depth) ) return;
    addBoundedVerifyDataRecursively(id,depth);
}

void SatSolver::addBoundedVerifyDataRecursively(const V3NetId& id, uint32_t& depth)
{
    assert( validNetId(id) );  
    if( existVerifyData(id,depth) ) return;
    const V3GateType type = _ntk->getGateType(id); assert(type < V3_XD);
    if( V3_PIO >= type ) add_PI_Formula(id,depth);
    else if( V3_FF == type ) {
        if(depth) { --depth; addBoundedVerifyDataRecursively(_ntk->getInputNetId(id,0), depth); ++depth; }
        add_FF_Formula(id, depth);
    }
    else if(AIG_FALSE >= type) {
        if(AIG_NODE == type) {
            addBoundedVerifyDataRecursively(_ntk->getInputNetId(id,0), depth);
            addBoundedVerifyDataRecursively(_ntk->getInputNetId(id,1), depth);
            add_AND_Formula(id,depth);
        }
        else {
            assert(AIG_FALSE == type);
            add_FALSE_Formula(id,depth);
        }
    }
    else {
        assert(0);
    }
}

const bool SatSolver::existVerifyData(const V3NetId& id, const uint32_t& depth)
{
    return getVerifyData(id, depth);
}

#endif
