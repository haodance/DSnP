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

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr(): _myHashMap(10000) { init(); }
   ~CirMgr() { clear(); } 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
       UnMap::const_iterator it;
       it = _totalList.find(gid);
       if(it != _totalList.end()) return it->second;
       return 0;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);
   void constructNet();
   void dfsTraversal(const GateList&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;


   //
   void init();
   void clear();
   void dfsOptimize(const GateList&, UnSet&);
   void dfsStrash(const GateList&, HashMap<HashKey, CirGateV>&);

private:
   ofstream           *_simLog;

   GateList _piList;
   GateList _poList;
   UnMap _totalList;

   GateList _dfsList;
   CirGatePairList _fbList;

   IdList _MILOA;

   vector<string> _comments;

   HashMap<HashKey, CirGateV> _myHashMap;
};

#endif // CIR_MGR_H
