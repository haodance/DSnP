/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
    UnSet uset;
    for(size_t i=0; i<_dfsList.size(); ++i)
        uset.insert(_dfsList[i]->_id);

    set<unsigned> deleted;
    UnMap::iterator it;
    UnSet::iterator setIt;
    for(it=_totalList.begin(); it!=_totalList.end(); ++it){
        setIt = uset.find(it->first);
        if(setIt == uset.end()){
            switch(it->second->getType()){
                case AIG_GATE:
                    {
                        AigGate* aigGate = dynamic_cast<AigGate*>(it->second);
                        for(int i=0; i<2; ++i){
                            CirGate* prevGate = aigGate->_faninList[i].gate();
                            switch(prevGate->getType()){
                                case AIG_GATE:
                                    {
                                        AigGate* aigGate = dynamic_cast<AigGate*>(prevGate);
                                        for(size_t j=0; j<aigGate->_fanoutList.size(); ++j)
                                            if(aigGate->_fanoutList[j] == it->second){
                                                aigGate->_fanoutList.erase(aigGate->_fanoutList.begin()+j);
                                                break;
                                            }

                                        break;
                                    }
                                case CONST_GATE:
                                    {
                                        ConstGate* constGate = dynamic_cast<ConstGate*>(prevGate);
                                        for(size_t j=0; j<constGate->_fanoutList.size(); ++j)
                                            if(constGate->_fanoutList[j] == it->second){
                                                constGate->_fanoutList.erase(constGate->_fanoutList.begin()+j);
                                                break;
                                            }
                                        break;
                                    }
                                case PI_GATE:
                                    {
                                        PiGate* piGate = dynamic_cast<PiGate*>(prevGate);
                                        for(size_t j=0; j<piGate->_fanoutList.size(); ++j)
                                            if(piGate->_fanoutList[j] == it->second){
                                                piGate->_fanoutList.erase(piGate->_fanoutList.begin()+j);
                                                break;
                                            }
                                        break;
                                    }
                                case UNDEF_GATE:
                                    {
                                        UndefGate* undefGate = dynamic_cast<UndefGate*>(prevGate);
                                        for(size_t j=0; j<undefGate->_fanoutList.size(); ++j)
                                            if(undefGate->_fanoutList[j] == it->second){
                                                undefGate->_fanoutList.erase(undefGate->_fanoutList.begin()+j);
                                                break;
                                            }
                                        break;
                                    }
                                default:
                                    break;
                            }
                        }
                        deleted.insert(it->first);
                        break;
                    }
                case UNDEF_GATE:
                    {
                        UndefGate* undefGate = dynamic_cast<UndefGate*>(it->second);
                        for(size_t i=0; i<undefGate->_fanoutList.size(); ++i){
                            CirGate* nextGate = undefGate->_fanoutList[i];
                            switch(nextGate->getType()){
                                case PO_GATE:
                                    {
                                        PoGate* poGate = dynamic_cast<PoGate*>(nextGate);
                                        poGate->_faninList.clear();
                                        poGate->_fanin.clear();
                                        break;
                                    }
                                default:
                                    break;
                            }
                        }
                        deleted.insert(it->first);
                        break;
                    }
                case CONST_GATE:
                    {
                        ConstGate* constGate = dynamic_cast<ConstGate*>(it->second);
                        constGate->_fanoutList.clear();
                        break;
                    }
                case PI_GATE:
                    {
                        PiGate* piGate = dynamic_cast<PiGate*>(it->second);
                        piGate->_fanoutList.clear();
                        break;
                    }
                default:
                    break;
            }
        }
    }

    set<unsigned>::iterator delIt;
    for(delIt=deleted.begin(); delIt!=deleted.end(); ++delIt){
        CirGate* tmp = getGate(*delIt);
        cout << "Sweeping: " << tmp->getTypeStr() << "(" << tmp->_id << ") removed..." << endl;
        delete tmp;
        tmp = 0;
        _totalList.erase(*delIt);
    }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
    UnSet deleted;
    dfsOptimize(_poList, deleted);

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

void CirMgr::dfsOptimize(const GateList& sinkList, UnSet& deleted){
    CirGate::setGlobalRef();
    for(size_t i=0; i<sinkList.size(); ++i)
        sinkList[i]->dfsOptimize(deleted);
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
