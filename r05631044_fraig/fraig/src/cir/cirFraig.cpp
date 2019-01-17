/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
    dfsStrash(_piList, _myHashMap);
    _myHashMap.clear();
}

void CirMgr::dfsStrash(const GateList& sinkList, HashMap<HashKey, CirGateV>& hash){
    UnSet dfsList;
    for(size_t i=0; i<_dfsList.size(); ++i)
        dfsList.insert(_dfsList[i]->_id);
    
    UnSet deleted;
    CirGate::setGlobalRef();
    for(size_t i=0; i<sinkList.size(); ++i)
        sinkList[i]->dfsStrash(hash, dfsList, deleted);

    UnSet::iterator delIt;
    for(delIt=deleted.begin(); delIt!=deleted.end(); ++delIt){
        CirGate* tmp = getGate(*delIt);
        delete tmp;
        tmp = 0;
        _totalList.erase(*delIt);
    }

    _dfsList.clear();
    _fbList.clear();
    dfsTraversal(_poList);
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
