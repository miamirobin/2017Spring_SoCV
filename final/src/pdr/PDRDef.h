/****************************************************************************
  FileName     [ PDRDef.h ]
  PackageName  [ pdr ]
  Synopsis     [ Define pdr basic classes ]
  Author       [ ]
  Copyright    [ Copyright(c) 2016 DVLab, GIEE, NTU, Taiwan ]
 ****************************************************************************/

#define show_address 0

#include <iostream>
#include <map>
#ifndef PDRDEF_H
#define PDRDEF_H

using namespace std;

class Value3 {
  // This is not dual rail encoding,
  // If the _dontCare is one it means it's X,
  // Otherwise it's decided by _bit
 public:
  Value3() : _bit(0), _dontCare(1) {}
  Value3(bool b, bool d): _bit(b), _dontCare(d) {}
  Value3(const Value3& a) {
    _bit = a._bit;
    _dontCare = a._dontCare;
  }
  Value3 operator & (Value3 a) const {
    if ((_bit == 0 && _dontCare == 0) || a == Value3(0, 0)) return Value3(0, 0);
    else if (a._dontCare || _dontCare) return Value3(0, 1);
    else return Value3(1, 0);
  }
  Value3 operator & (bool a) const {
    if (a == 0) return Value3(0, 0);
    else if (_dontCare) return Value3(0, 1);
    else return Value3(_bit, 0);
  }
  Value3 operator | (Value3 a) const {
    if ((_bit == 1 && _dontCare == 0) || a == Value3(1,0)) return Value3(1, 0);
    else if (a._dontCare || _dontCare) return Value3(0, 1);
    else return Value3(0, 0);
  }
  Value3 operator | (bool a) const {
    if (a) return Value3(1, 0);
    else if (_dontCare) return Value3(0, 1);
    else return Value3(_bit, 0);
  }
  Value3 operator ~ () const {
    if (_dontCare) return Value3(0, 1);
    else return Value3(!_bit, 0);
  }
  bool operator == (const Value3& a) const {
    if (_dontCare ^ a._dontCare) return false;
    else if (_dontCare && a._dontCare) return true;
    else if (_bit == a._bit) return true;
    else return false;
  }
  bool operator != (const Value3& a) const {
    return !((*this) == a);
  }
  bool _bit;
  bool _dontCare;
};

class Cube {
 public:
  
  Cube(){
    // cube is all zeros for default constructor
    //_latchValues = new Value3[_L];
     //map<unsigned, Value3> mapLatch;
   
    for (unsigned i = 0; i < _L; ++i) {
      
      
       mapLatch[i] = Value3(0,0);
    }
  }
  Cube(bool* b) {
    //map<unsigned, Value3> map;
    
    for (unsigned i = 0; i < _L; ++i){
      
      mapLatch[i]._bit=b[i];
      
    }
  }
  Cube(bool* b, bool* d) {
   // map<unsigned, Value3> map;
   
    for (unsigned i = 0; i < _L; ++i) {
      
      mapLatch[i] = Value3(b[i],d[i]);
    }
  }
  Cube(Cube* c) {
    if (c->mapLatch.size()>0) {
      //map<unsigned, Value3> mapLatch;
      map<unsigned, Value3>::iterator iter;
      
      for (unsigned i = 0; i < _L; ++i) {
        iter = c->mapLatch.find(i);
        if (iter!=c->mapLatch.end()){
        	mapLatch[i] =  iter->second;
        }
      }
    } else {
       mapLatch.clear();
    }
  }
  Cube( Cube& c) {
     Cube cc= c;
    if (cc.mapLatch.size()>0) {
      //map<unsigned, Value3> mapLatch;
      map<unsigned, Value3>::iterator iter;
          
      for (unsigned i = 0; i < _L; ++i) {
        iter = (cc.mapLatch).find(i); 
        if (iter!=(cc.mapLatch).end()){
        	mapLatch[i] =  iter->second;
        }
      }

    } else {
      mapLatch.clear();
    }
  }
  ~Cube() {
    if (mapLatch.size()>0) {
      
      mapLatch.clear();
    }
    
  }
  bool subsumes(Cube* s)  {
    map<unsigned, Value3>::iterator iter ;
    map<unsigned, Value3>::iterator iter2;
   for(iter2 = mapLatch.begin(); iter2!= mapLatch.end(); iter2++) {
	
        iter=s->mapLatch.find(iter2->first);
        if (iter!=s->mapLatch.end()){
        	if(iter2->second!=iter->second){return false;}
        }
        else {return false;}
    }
    return true;
        	
 /*     for (unsigned i = 0; i < _L; ++i) {
      
      iter = mapLatch.find(i);
      if (iter!=mapLatch.end()){
      	if (!iter->second._dontCare) {
        	iter2 =s->mapLatch.find(i);
                if (iter2!=s->mapLatch.end()){
        		if (iter2->second._dontCare) return false;
        		if (iter2->second._bit != iter->second._bit) return false;

                }
                else {return true;}
      	}
      }
    }
                   */
  }
  void show() {
    // debug fuction
    map<unsigned, Value3>::iterator iter;
    for (unsigned i = _L - 1; i != 0; --i) {
      iter = mapLatch.find(i);
      
      if (iter!=mapLatch.end()){
      	if (iter->second._dontCare) cerr << "X";
     	else cerr << ((iter->second._bit) ? "1" : "0");
      }
      else {cout<<" ";}
    }
    iter = mapLatch.find(0);
    if (iter!=mapLatch.end()){
    	if (iter->second._dontCare) cerr << "X";
    	else cerr << ((iter->second._bit) ? "1" : "0");
    	cerr << endl;
    }
    else {cout<<" "<<endl;}
  }
  static unsigned _L;               // latch size
  Value3*         _latchValues;     // latch values
  map<unsigned, Value3> mapLatch;   // latch values in hash table
};
 

class TCube
{
 public:
  TCube(): _cube(NULL), _frame(-1) {
    cerr << "NULL constructor" << endl;
  }
  TCube(Cube* c, unsigned d): _cube(c), _frame((int)d) {
    if (show_address) {
      cerr << "default constructor" << endl;
      cerr << c << endl;
    }
  }
  TCube(const TCube& t) {
    if (show_address) {
      cerr << "copy constructor" << endl;
      cerr << _cube << endl;
      cerr << t._cube << endl;
    }
    _cube = t._cube;
    _frame = t._frame;
  }
  ~TCube(){
    if (show_address) cerr << "destructor" << endl;
  }
  TCube& operator = (const TCube& t){
    if (show_address) {
      cerr << "= constructor" << endl;
      cerr << _cube << endl;
      cerr << t._cube << endl;
    }
    _cube = t._cube;
    _frame = t._frame;
    return (*this);
  }
  friend bool operator > (const TCube& l, const TCube& r) { return l._frame > r._frame; }

  Cube* _cube;
  int   _frame; // -1 = frame_null, INT_MAX = INF
};

#endif
