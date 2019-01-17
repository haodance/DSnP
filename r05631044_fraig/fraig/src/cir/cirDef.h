/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include "myHashMap.h"
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <iomanip>

using namespace std;

// TODO: define your own typedef or enum

class CirGate;
class CirMgr;
class SatSolver;

typedef vector<CirGate*>                        GateList;
typedef vector<unsigned>                        IdList;
typedef vector< pair<CirGate*, CirGate*> >      CirGatePairList;
typedef unordered_set<unsigned>                 UnSet;
typedef unordered_map<unsigned, CirGate*>       UnMap;

enum GateType{
    UNDEF_GATE = 0,
    PI_GATE = 1,
    PO_GATE = 2,
    AIG_GATE = 3,
    CONST_GATE = 4,

    TOT_GATE
};

#endif // CIR_DEF_H
