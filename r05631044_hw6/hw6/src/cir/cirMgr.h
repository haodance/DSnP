/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace std;

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr() {
       CirGate* gate = new ConstGate;
       gate->setIdLineColumn(0, 0, 0);
       _totalList.insert(make_pair(gate->_id, gate));

       _piList.reserve(10000);
       _poList.reserve(10000);
       _totalList.reserve(100000);
       _dfsList.reserve(100000);
   }
   ~CirMgr() { clear(); }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
       unordered_map<unsigned, CirGate*>::const_iterator it;
       it = _totalList.find(gid);
       if(it != _totalList.end())
           return it->second;
       return 0;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);
   void constructNet();
   void dfsTraversal(const GateList&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;


   //Self-defined member functions
   void clear();

private:
   GateList _piList;
   GateList _poList;
   unordered_map<unsigned, CirGate*> _totalList;

   GateList _dfsList;
   vector< pair<CirGate*, CirGate*> > _fbList;

   IdList _MILOA;

   vector<string> _comments;
};

#endif // CIR_MGR_H
