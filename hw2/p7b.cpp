/****************************************************************************
  FileName     [ testBdd.cpp ]
  PackageName  [ ]
  Synopsis     [ Define main() ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "bddNode.h"
#include "bddMgr.h"

using namespace std;

/**************************************************************************/
/*                        Define Global BDD Manager                       */
/**************************************************************************/
BddMgr bm;

/**************************************************************************/
/*                    Define Static Function Prototypes                   */
/**************************************************************************/
static void initBdd(size_t nSupports, size_t hashSize, size_t cacheSize);


/**************************************************************************/
/*                             Define main()                              */
/**************************************************************************/
int
main()
{
   initBdd(6, 127, 61);

   /*-------- THIS IS JUST A TEST CODE ---------*/
   BddNode a(bm.getSupport(1));
   BddNode b(bm.getSupport(2));
   BddNode c(bm.getSupport(3));
   BddNode d(bm.getSupport(4));
   BddNode e(bm.getSupport(5));
   BddNode f(bm.getSupport(6));

    
   BddNode g1 = d & b;
   cout << "g1:" << endl;
   cout << g1 << endl;

   BddNode g2 = ~c & e;
    cout << "g2:" << endl;
   cout << g2 << endl;

   BddNode g3 = a & b;
    cout << "g3:" << endl;
   cout << g3 << endl;
    
   BddNode g4 = c & g1;
    cout << "g4:" << endl;
   cout << g4 << endl;
    
   BddNode g5 = g1 | g2;
    cout << "g5:" << endl;
   cout << g5 << endl;
    
   BddNode g6 = g2 | d;
    cout << "g6:" << endl;
   cout << g6 << endl;
    
   BddNode g7 = g6 & g3;
    cout << "g7:" << endl;
   cout << g7 << endl;
    
   BddNode g8 = g4 | g7;
    cout << "g8:" << endl;
   cout << g8 << endl;
   
   BddNode g9 = g8 & f;
    cout << "g9:" << endl;
   cout << g9 << endl;
    
    BddNode Diff = (g8 & (g5 & BddNode::_zero))^(g8 & (g5 & BddNode::_one));
    BddNode Redundancy1=Diff & ~f;
    BddNode Redundancy0=Diff & f;
   
    
    cout << "p7b g9" << endl;
    BddNode _g2=bm.restrict(~e,~c);  //(1)
    BddNode _g6=_g2|~d;              //(2)
    BddNode _g7=bm.restrict(_g6,g3); //(3)
    BddNode _g8=bm.restrict(g4,_g7); //(4)
    BddNode _g9=_g8&f;               //(5)
    cout<<_g9<<endl;
    
  

   ofstream oofile("_g9.dot");
   _g9.drawBdd("_g9", oofile);
   system("dot -o _g9.png -Tpng _g9.dot");
  
   /*----------- END OF TEST CODE ------------*/
}


/**************************************************************************/
/*                          Define Static Functions                       */
/**************************************************************************/
static void
initBdd(size_t nin, size_t h, size_t c)
{
   BddNode::_debugBddAddr = true;
   BddNode::_debugRefCount = true;

//   bm.reset();
   bm.init(nin, h, c);
}

