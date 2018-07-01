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
  

    
   BddNode g1 = ~a & b & ~c;
   cout << "g1:" << endl;
   cout << g1 << endl;

   BddNode g2 = a & ~c & d;
    cout << "g2:" << endl;
   cout << g2 << endl;

   BddNode g3 = a & c& ~d;
    cout << "g3:" << endl;
   cout << g3 << endl;
    
   BddNode g4 = ~b & ~c&~d;
    cout << "g4:" << endl;
   cout << g4 << endl;
    
   BddNode g5 = g1 | g2 |g3|g4;
    cout << "g5:" << endl;
   cout << g5 << endl;
    
  
    
   //BddNode i = f ^ (c | d); // f ^ g;
  //cout << i << endl;

   //BddNode j = ~a | ~b;
   //cout << j << endl;

   //cout << "KK" << endl;
   //BddNode k = ( (a|b) ^ (d&e) );
   //cout << k << endl;
//   cout << k.getLeftCofactor(1) << endl;
   //cout << k.getLeftCofactor(2) << endl;
//   cout << k.getRightCofactor(4) << endl;
  // cout << k.getRightCofactor(2) << endl;
  // BddNode l1 = k.exist(2);
  // cout << l1 << endl;
   //BddNode l2 = k.getLeftCofactor(2) | k.getRightCofactor(2);
   //cout << l2 << endl;

   ofstream ofile("g5.dot");
   g9.drawBdd("g5", ofile);
   system("dot -o g5.png -Tpng g5.dot");

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

