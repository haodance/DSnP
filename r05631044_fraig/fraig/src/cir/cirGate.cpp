/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;
class HashKey;

unsigned CirGate::_globalRef = 0;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
    for(int i=0; i<80; ++i) cout << "=";
    cout << endl;

    cout << "= " << this->getTypeStr() << "(" << this->_id << ")";
    if(!this->_symbol.empty()) cout << "\"" << this->_symbol << "\"";
    cout << ", line " << this->_line << endl;

    cout << "= FECs:" << endl;

    cout << "= Value:" << "00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000" << endl;

    for(int i=0; i<80; ++i) cout << "=";
    cout << endl;
}

void
CirGate::reportFanin(int level)
{
   assert (level >= 0);

   UnSet uSet;
   printFanin(this, level, 0, uSet, 0);
}

void
CirGate::reportFanout(int level)
{
   assert (level >= 0);

   UnSet uSet;
   printFanout(this, level, 0, uSet, 0);
}

void CirGate::printGate(){
    cout << setw(4) << left << getTypeStr() << _id;
    switch(getType()){
        case PI_GATE:
            {
                if(!_symbol.empty()) cout << " (" << _symbol << ")";
                break;
            }
        case PO_GATE:
            {
                cout << " ";
                PoGate* poGate = dynamic_cast<PoGate*>(this);
                if(poGate->_faninList[0].isInv()) cout << "!";
                cout << poGate->_faninList[0].gate()->_id;
                if(!_symbol.empty()) cout << " (" << _symbol << ")";
                break;
            }
        case AIG_GATE:
            {
                AigGate* aigGate = dynamic_cast<AigGate*>(this);
                for(int i=0; i<2; ++i){
                    cout << " ";
                    if(aigGate->_faninList[i].isInv()) cout << "!";
                    cout << aigGate->_faninList[i].gate()->_id;
                }
                break;
            }
        default:
            break;
    }

    cout << endl;
}

void CirGate::printFanin(CirGate* gate, int& level, int levelCount, UnSet& set, CirGate* ogGate){
    if(levelCount > level) return;

    ++levelCount;
    for(int i=0; i<levelCount-1; ++i) cout << "  ";

    UnSet::iterator it;
    switch(gate->getType()){
        case UNDEF_GATE:
        case CONST_GATE:
        case PI_GATE:
            {
                if(ogGate == 0) {}
                else if(ogGate->getTypeStr() == "PO"){
                    if(dynamic_cast<PoGate*>(ogGate)->_faninList[0].isInv()) cout << "!";
                }
                else if(ogGate->getTypeStr() == "AIG"){
                    AigGate* aigGate = dynamic_cast<AigGate*>(ogGate);
                    if(aigGate->_faninList[0].gate()==gate && aigGate->_faninList[0].isInv()) cout << "!";
                    else if(aigGate->_faninList[1].gate()==gate && aigGate->_faninList[1].isInv()) cout << "!";
                }

                if(ogGate) set.insert(ogGate->_id);
                cout << gate->getTypeStr() << " " << gate->_id << endl;

                break;
            }
        case PO_GATE:
            {
                it = set.find(gate->_id);
                if(it != set.end())
                    cout << "PO " << gate->_id << " (*)" << endl;
                else{
                    cout << "PO " << gate->_id << endl;
                    if(ogGate) set.insert(ogGate->_id);
                    PoGate* poGate = dynamic_cast<PoGate*>(gate);
                    printFanin(poGate->_faninList[0].gate(), level, levelCount, set, gate);
                }

                break;
            }
        case AIG_GATE:
            {
                if(ogGate == 0) {}
                else if(ogGate->getTypeStr() == "PO"){
                    if(dynamic_cast<PoGate*>(ogGate)->_faninList[0].isInv()) cout << "!";
                }
                else if(ogGate->getTypeStr() == "AIG"){
                    AigGate* aigGate = dynamic_cast<AigGate*>(ogGate);
                    if(aigGate->_faninList[0].gate()==gate && aigGate->_faninList[0].isInv()) cout << "!";
                    else if(aigGate->_faninList[1].gate()==gate && aigGate->_faninList[1].isInv()) cout << "!";
                }

                it = set.find(gate->_id);
                if(it != set.end())
                    cout << "AIG " << gate->_id << " (*)" << endl;
                else{
                    cout << "AIG " << gate->_id << endl;
                    if(ogGate) set.insert(ogGate->_id);
                    AigGate* aigGate = dynamic_cast<AigGate*>(gate);
                    for(int i=0; i<2; ++i)
                        printFanin(aigGate->_faninList[i].gate(), level, levelCount, set, gate);
                }

                break;
            }
        default:
            break;
    }
}

void CirGate::printFanout(CirGate* gate, int& level, int levelCount, UnSet& set, CirGate* ogGate){
    if(levelCount > level) return;

    ++levelCount;
    for(int i=0; i<levelCount-1; ++i) cout << "  ";

    UnSet::iterator it;
    switch(gate->getType()){
        case PO_GATE:
            {
                if(ogGate == 0) {}
                else if(dynamic_cast<PoGate*>(gate)->_faninList[0].isInv()) cout << "!";

                if(ogGate) set.insert(ogGate->_id);
                cout << "PO " << gate->_id << endl;

                break;
            }
        case AIG_GATE:
            {
                AigGate* aigGate = dynamic_cast<AigGate*>(gate);
                if(ogGate == 0) {}
                else if(aigGate->_faninList[0].gate()==ogGate && aigGate->_faninList[0].isInv()) cout << "!";
                else if(aigGate->_faninList[1].gate()==ogGate && aigGate->_faninList[1].isInv()) cout << "!";

                it = set.find(gate->_id);
                if(it != set.end())
                    cout << "AIG " << gate->_id << " (*)" << endl;
                else{
                    cout << "AIG " << gate->_id << endl;
                    if(ogGate) set.insert(ogGate->_id);
                    for(int i=aigGate->_fanoutList.size()-1; i>=0; --i)
                        printFanout(aigGate->_fanoutList[i], level, levelCount, set, gate);
                }

                break;
            }
        case PI_GATE:
            {
                it = set.find(gate->_id);
                if(it != set.end())
                    cout << "PI " << gate->_id << " (*)" << endl;
                else{
                    cout << "PI " << gate->_id << endl;
                    if(ogGate) set.insert(ogGate->_id);
                    PiGate* piGate = dynamic_cast<PiGate*>(gate);
                    for(int i=piGate->_fanoutList.size()-1; i>=0; --i)
                        printFanout(piGate->_fanoutList[i], level, levelCount, set, gate);
                }

                break;
            }
        case CONST_GATE:
            {
                it = set.find(gate->_id);
                if(it != set.end())
                    cout << "CONST " << gate->_id << " (*)" << endl;
                else{
                    cout << "CONST " << gate->_id << endl;
                    if(ogGate) set.insert(ogGate->_id);
                    ConstGate* constGate = dynamic_cast<ConstGate*>(gate);
                    for(int i=constGate->_fanoutList.size()-1; i>=0; --i)
                        printFanout(constGate->_fanoutList[i], level, levelCount, set, gate);
                }

                break;
            }
        case UNDEF_GATE:
            {
                it = set.find(gate->_id);
                if(it != set.end())
                    cout << "UNDEF " << gate->_id << " (*)" << endl;
                else{
                    cout << "UNDEF " << gate->_id << endl;
                    if(ogGate) set.insert(ogGate->_id);
                    UndefGate* undefGate = dynamic_cast<UndefGate*>(gate);
                    for(int i=undefGate->_fanoutList.size()-1; i>=0; --i)
                        printFanout(undefGate->_fanoutList[i], level, levelCount, set, gate);
                }

                break;
            }
        default:
            break;
    }
}

void CirGate::dfsTraversal(GateList& dfsList, CirGatePairList& fbList){
    assert(this != 0);

    switch(this->getType()){
        case UNDEF_GATE:
        case PI_GATE:
        case CONST_GATE:
            {
                dfsList.push_back(this);
                break;
            }
        case PO_GATE:
            {
                PoGate* poGate = dynamic_cast<PoGate*>(this);
                for(size_t i=0; i<poGate->_faninList.size(); ++i){
                    if(!poGate->_faninList[i].gate()->isGlobalRef()){
                        poGate->_faninList[i].gate()->setToGlobalRef();
                        poGate->_active = true;
                        poGate->_faninList[i].gate()->dfsTraversal(dfsList, fbList);
                        poGate->_active = false;
                    }
                    else if(poGate->_faninList[i].gate()->_active)
                        fbList.push_back(make_pair(this, poGate->_faninList[i].gate()));
                }

                dfsList.push_back(this);
                break;
            }
        case AIG_GATE:
            {
                AigGate* aigGate = dynamic_cast<AigGate*>(this);
                for(size_t i=0; i<aigGate->_faninList.size(); ++i){
                    if(!aigGate->_faninList[i].gate()->isGlobalRef()){
                        aigGate->_faninList[i].gate()->setToGlobalRef();
                        aigGate->_active = true;
                        aigGate->_faninList[i].gate()->dfsTraversal(dfsList, fbList);
                        aigGate->_active = false;
                    }
                    else if(aigGate->_faninList[i].gate()->_active)
                        fbList.push_back(make_pair(this, aigGate->_faninList[i].gate()));
                }

                dfsList.push_back(this);
                break;
            }
        default:
            break;
    }
}

void CirGate::dfsOptimize(UnSet& deleted){
    switch(this->getType()){
        case AIG_GATE:
            {
                AigGate* aigGate = dynamic_cast<AigGate*>(this);
                for(size_t i=0; i<aigGate->_faninList.size(); ++i){
                    if(!aigGate->_faninList[i].gate()->isGlobalRef()){
                        aigGate->_faninList[i].gate()->setToGlobalRef();
                        aigGate->_active = true;
                        aigGate->_faninList[i].gate()->dfsOptimize(deleted);
                        aigGate->_active = false;
                    }
                }
                aigGate->simplifyConst(deleted);
                aigGate->simplifyFanin(deleted);

                break;
            }
        case PO_GATE:
            {
                PoGate* poGate = dynamic_cast<PoGate*>(this);
                for(size_t i=0; i<poGate->_faninList.size(); ++i){
                    if(!poGate->_faninList[i].gate()->isGlobalRef()){
                        poGate->_faninList[i].gate()->setToGlobalRef();
                        poGate->_active = true;
                        poGate->_faninList[i].gate()->dfsOptimize(deleted);
                        poGate->_active = false;
                    }
                }

                break;
            }
        default:
            break;
    }
}

void AigGate::simplifyConst(UnSet& deleted){
    AigGate* aigGate = dynamic_cast<AigGate*>(this);
    bool hasConst0 = false, hasConst1 = false;
    unsigned id0 = -1, id1 = -1;

    for(int i=0; i<2; ++i)
        if(aigGate->_faninList[i].gate()->getTypeStr() == "CONST"){
            if(aigGate->_faninList[i].isInv()) { hasConst1 = true; id1 = i; }
            else { hasConst0 = true; id0 = i; }
        }

    if(hasConst0){
        assert(id0 >= 0);
        
        UnSet::iterator it = deleted.find(this->_id);
        if(it == deleted.end()){
            deleted.insert(this->_id);
            cout << "Simplifying: 0 merging " << this->_id << "..." << endl;
        }

        // -->
        ConstGate* constGate = dynamic_cast<ConstGate*>(aigGate->_faninList[id0].gate());
        for(size_t i=0; i<constGate->_fanoutList.size(); ++i)
            if(constGate->_fanoutList[i] == this){
                constGate->_fanoutList.erase(constGate->_fanoutList.begin()+i);
                break;
            }
        for(size_t i=0; i<aigGate->_fanoutList.size(); ++i)
            constGate->_fanoutList.push_back(aigGate->_fanoutList[i]);

        // <--
        for(size_t i=0; i<aigGate->_fanoutList.size(); ++i){
            switch(aigGate->_fanoutList[i]->getType()){
                case AIG_GATE:
                    {
                        AigGate* nextGate = dynamic_cast<AigGate*>(aigGate->_fanoutList[i]);
                        for(size_t j=0; j<2; ++j)
                            if(nextGate->_faninList[j].gate() == this){
                                if(nextGate->_faninList[j].isInv()){
                                    nextGate->_faninList[j].setGate(aigGate->_faninList[id0].gate(), true);
                                    nextGate->_fanin[j] = 1;
                                }
                                else{
                                    nextGate->_faninList[j].setGate(aigGate->_faninList[id0].gate(), false);
                                    nextGate->_fanin[j] = 0;
                                }
                            }

                        break;
                    }
                case PO_GATE:
                    {
                        PoGate* nextGate = dynamic_cast<PoGate*>(aigGate->_fanoutList[i]);
                        if(nextGate->_faninList[0].gate() == this){
                            if(nextGate->_faninList[0].isInv()){
                                nextGate->_faninList[0].setGate(aigGate->_faninList[id0].gate(), true);
                                nextGate->_fanin[0] = 1;
                            }
                            else{
                                nextGate->_faninList[0].setGate(aigGate->_faninList[id0].gate(), false);
                                nextGate->_fanin[0] = 1;
                            }
                        }

                        break;
                    }
                default:
                    break;
            }
        }
    }

    else if(hasConst1){
        assert(id1 >= 0);

        UnSet::iterator it = deleted.find(this->_id);
        if(it == deleted.end()){
            deleted.insert(this->_id);
            cout << "Simplifying: " << aigGate->_faninList[1-id1].gate()->_id << " merging ";
            if(aigGate->_faninList[1-id1].isInv()) cout << "!";
            cout << this->_id << "..." << endl;
        }

        // -->
        ConstGate* constGate = dynamic_cast<ConstGate*>(aigGate->_faninList[id1].gate());
        for(size_t i=0; i<constGate->_fanoutList.size(); ++i)
            if(constGate->_fanoutList[i] == this){
                constGate->_fanoutList.erase(constGate->_fanoutList.begin()+i);
                break;
            }

        switch(aigGate->_faninList[1-id1].gate()->getType()){
            case UNDEF_GATE:
                {
                    UndefGate* prevGate = dynamic_cast<UndefGate*>(aigGate->_faninList[1-id1].gate());
                    for(size_t i=0; i<prevGate->_fanoutList.size(); ++i)
                        if(prevGate->_fanoutList[i] == this){
                            prevGate->_fanoutList.erase(prevGate->_fanoutList.begin()+i);
                            break;
                        }

                    for(size_t i=0; i<aigGate->_fanoutList.size(); ++i)
                        prevGate->_fanoutList.push_back(aigGate->_fanoutList[i]);
                    
                    break;
                }
            case PI_GATE:
                {
                    PiGate* prevGate = dynamic_cast<PiGate*>(aigGate->_faninList[1-id1].gate());
                    for(size_t i=0; i<prevGate->_fanoutList.size(); ++i)
                        if(prevGate->_fanoutList[i] == this){
                            prevGate->_fanoutList.erase(prevGate->_fanoutList.begin()+i);
                            break;
                        }

                    for(size_t i=0; i<aigGate->_fanoutList.size(); ++i)
                        prevGate->_fanoutList.push_back(aigGate->_fanoutList[i]);

                    break;
                }
            case AIG_GATE:
                {
                    AigGate* prevGate = dynamic_cast<AigGate*>(aigGate->_faninList[1-id1].gate());
                    for(size_t i=0; i<prevGate->_fanoutList.size(); ++i)
                        if(prevGate->_fanoutList[i] == this){
                            prevGate->_fanoutList.erase(prevGate->_fanoutList.begin()+i);
                            break;
                        }

                    for(size_t i=0; i<aigGate->_fanoutList.size(); ++i)
                        prevGate->_fanoutList.push_back(aigGate->_fanoutList[i]);

                    break;
                }
            case CONST_GATE:
                {
                    ConstGate* prevGate = dynamic_cast<ConstGate*>(aigGate->_faninList[1-id1].gate());
                    for(size_t i=0; i<prevGate->_fanoutList.size(); ++i)
                        if(prevGate->_fanoutList[i] == this){
                            prevGate->_fanoutList.erase(prevGate->_fanoutList.begin()+i);
                            break;
                        }

                    for(size_t i=0; i<aigGate->_fanoutList.size(); ++i)
                        prevGate->_fanoutList.push_back(aigGate->_fanoutList[i]);

                    break;
                }
            default:
                break;
        }

        // <--
        for(size_t i=0; i<aigGate->_fanoutList.size(); ++i){
            switch(aigGate->_fanoutList[i]->getType()){
                case AIG_GATE:
                    {
                        AigGate* nextGate = dynamic_cast<AigGate*>(aigGate->_fanoutList[i]);
                        for(int j=0; j<2; ++j)
                            if(nextGate->_faninList[j].gate() == this){
                                if((nextGate->_faninList[j].isInv() && aigGate->_faninList[1-id1].isInv())
                                        || (!nextGate->_faninList[j].isInv() && !aigGate->_faninList[1-id1].isInv())){
                                    nextGate->_faninList[j].setGate(aigGate->_faninList[1-id1].gate(), false);
                                    nextGate->_fanin[j] = 2*aigGate->_faninList[1-id1].gate()->_id;
                                }
                                else{
                                    nextGate->_faninList[j].setGate(aigGate->_faninList[1-id1].gate(), true);
                                    nextGate->_fanin[j] = 2*aigGate->_faninList[1-id1].gate()->_id+1;
                                }
                            }

                        break;
                    }
                case PO_GATE:
                    {
                        PoGate* nextGate = dynamic_cast<PoGate*>(aigGate->_fanoutList[i]);
                        assert(nextGate->_faninList[0].gate() == this);
                        if((nextGate->_faninList[0].isInv() && aigGate->_faninList[1-id1].isInv())
                                || (!nextGate->_faninList[0].isInv() && !aigGate->_faninList[1-id1].isInv())){
                            nextGate->_faninList[0].setGate(aigGate->_faninList[1-id1].gate(), false);
                            nextGate->_fanin[0] = 2*aigGate->_faninList[1-id1].gate()->_id;
                        }
                        else{
                            nextGate->_faninList[0].setGate(aigGate->_faninList[1-id1].gate(), true);
                            nextGate->_fanin[0] = 2*aigGate->_faninList[1-id1].gate()->_id+1;
                        }

                        break;
                    }
                default:
                    break;
            }
        }
    }
}

void AigGate::simplifyFanin(UnSet& deleted){
    AigGate* aigGate = dynamic_cast<AigGate*>(this);
    // Identical fanins
    if((aigGate->_faninList[0].gate() == aigGate->_faninList[1].gate())
        && (aigGate->_faninList[0].isInv() == aigGate->_faninList[1].isInv())){
        UnSet::iterator it = deleted.find(this->_id);
        if(it == deleted.end()){
            deleted.insert(this->_id);
            cout << "Simplifying: " << aigGate->_faninList[0].gate()->_id << " merging ";
            if(aigGate->_faninList[0].isInv()) cout << "!";
            cout << this->_id << "..." << endl;
        }
        bool inv = aigGate->_faninList[0].isInv();
        
        // -->
        switch(aigGate->_faninList[0].gate()->getType()){
            case CONST_GATE:
                {
                    ConstGate* prevGate = dynamic_cast<ConstGate*>(aigGate->_faninList[0].gate());
                    for(size_t i=0; i<prevGate->_fanoutList.size(); ++i)
                        if(prevGate->_fanoutList[i] == this){
                            prevGate->_fanoutList.erase(prevGate->_fanoutList.begin()+i);
                            break;
                        }
                    for(size_t i=0; i<aigGate->_fanoutList.size(); ++i)
                        prevGate->_fanoutList.push_back(aigGate->_fanoutList[i]);

                    break;
                }
            case PI_GATE:
                {
                    PiGate* prevGate = dynamic_cast<PiGate*>(aigGate->_faninList[0].gate());
                    for(size_t i=0; i<prevGate->_fanoutList.size(); ++i)
                        if(prevGate->_fanoutList[i] == this){
                            prevGate->_fanoutList.erase(prevGate->_fanoutList.begin()+i);
                            break;
                        }
                    for(size_t i=0; i<aigGate->_fanoutList.size(); ++i)
                        prevGate->_fanoutList.push_back(aigGate->_fanoutList[i]);

                    break;
                }
            case AIG_GATE:
                {
                    AigGate* prevGate = dynamic_cast<AigGate*>(aigGate->_faninList[0].gate());
                    for(size_t i=0; i<prevGate->_fanoutList.size(); ++i)
                        if(prevGate->_fanoutList[i] == this){
                            prevGate->_fanoutList.erase(prevGate->_fanoutList.begin()+i);
                            break;
                        }
                    for(size_t i=0; i<aigGate->_fanoutList.size(); ++i)
                        prevGate->_fanoutList.push_back(aigGate->_fanoutList[i]);

                    break;
                }
            case UNDEF_GATE:
                {
                    UndefGate* prevGate = dynamic_cast<UndefGate*>(aigGate->_faninList[0].gate());
                    for(size_t i=0; i<prevGate->_fanoutList.size(); ++i)
                        if(prevGate->_fanoutList[i] == this){
                            prevGate->_fanoutList.erase(prevGate->_fanoutList.begin()+i);
                            break;
                        }
                    for(size_t i=0; i<aigGate->_fanoutList.size(); ++i)
                        prevGate->_fanoutList.push_back(aigGate->_fanoutList[i]);

                    break;
                }
            default:
                break;
        }


        // <--
        for(size_t i=0; i<aigGate->_fanoutList.size(); ++i){
            switch(aigGate->_fanoutList[i]->getType()){
                case PO_GATE:
                    {
                        PoGate* nextGate = dynamic_cast<PoGate*>(aigGate->_fanoutList[i]);
                        if((nextGate->_faninList[0].isInv() && inv)
                            || (!nextGate->_faninList[0].isInv() && !inv)){
                            nextGate->_faninList[0].setGate(aigGate->_faninList[0].gate(), false);
                            nextGate->_fanin[0] = 2*aigGate->_faninList[0].gate()->_id;
                        }
                        else{
                            nextGate->_faninList[0].setGate(aigGate->_faninList[0].gate(), true);
                            nextGate->_fanin[0] = 2*aigGate->_faninList[0].gate()->_id+1;
                        }

                        break;
                    }
                case AIG_GATE:
                    {
                        AigGate* nextGate = dynamic_cast<AigGate*>(aigGate->_fanoutList[i]);
                        for(int j=0; j<2; ++j){
                            if(nextGate->_faninList[j].gate() == this){
                                if((nextGate->_faninList[j].isInv() && inv)
                                    || (!nextGate->_faninList[j].isInv() && !inv)){
                                    nextGate->_faninList[j].setGate(aigGate->_faninList[0].gate(), false);
                                    nextGate->_fanin[j] = 2*aigGate->_faninList[0].gate()->_id;
                                }
                                else{
                                    nextGate->_faninList[j].setGate(aigGate->_faninList[0].gate(), true);
                                    nextGate->_fanin[j] = 2*aigGate->_faninList[0].gate()->_id+1;
                                }
                            }
                        }

                        break;
                    }
                default:
                    break;
            }
        }
    }


    //Inverted fanins
    else if((aigGate->_faninList[0].gate() == aigGate->_faninList[1].gate())
            && (aigGate->_faninList[0].isInv() != aigGate->_faninList[1].isInv())){
        UnSet::iterator it = deleted.find(this->_id);
        if(it == deleted.end()){
            deleted.insert(this->_id);
            cout << "Simplifying: 0 merging " << this->_id << "..." << endl;
        }

        // -->
        switch(aigGate->_faninList[0].gate()->getType()){
            case PI_GATE:
                {
                    PiGate* prevGate = dynamic_cast<PiGate*>(aigGate->_faninList[0].gate());
                    for(size_t i=0; i<prevGate->_fanoutList.size(); ++i)
                        if(prevGate->_fanoutList[i] == this){
                            prevGate->_fanoutList.erase(prevGate->_fanoutList.begin()+i);
                            break;
                        }

                    break;
                }
            case UNDEF_GATE:
                {
                    UndefGate* prevGate = dynamic_cast<UndefGate*>(aigGate->_faninList[0].gate());
                    for(size_t i=0; i<prevGate->_fanoutList.size(); ++i)
                        if(prevGate->_fanoutList[i] == this){
                            prevGate->_fanoutList.erase(prevGate->_fanoutList.begin()+i);
                            break;
                        }

                    break;
                }
            case AIG_GATE:
                {
                    AigGate* prevGate = dynamic_cast<AigGate*>(aigGate->_faninList[0].gate());
                    for(size_t i=0; i<prevGate->_fanoutList.size(); ++i)
                        if(prevGate->_fanoutList[i] == this){
                            prevGate->_fanoutList.erase(prevGate->_fanoutList.begin()+i);
                            break;
                        }

                    break;
                }
            default:
                break;
        }

        ConstGate* constGate = dynamic_cast<ConstGate*>(cirMgr->getGate(0));
        for(size_t i=0; i<aigGate->_fanoutList.size(); ++i)
            constGate->_fanoutList.push_back(aigGate->_fanoutList[i]);


        // <--
        for(size_t i=0; i<aigGate->_fanoutList.size(); ++i){
            switch(aigGate->_fanoutList[i]->getType()){
                case PO_GATE:
                    {
                        PoGate* nextGate = dynamic_cast<PoGate*>(aigGate->_fanoutList[i]);
                        if(nextGate->_faninList[0].isInv()){
                            nextGate->_faninList[0].setGate(cirMgr->getGate(0), true);
                            nextGate->_fanin[0] = 1;
                        }
                        else{
                            nextGate->_faninList[0].setGate(cirMgr->getGate(0), false);
                            nextGate->_fanin[0] = 0;
                        }
                        break;
                    }
                case AIG_GATE:
                    {
                        AigGate* nextGate = dynamic_cast<AigGate*>(aigGate->_fanoutList[i]);
                        for(int i=0; i<2; ++i){
                            if(nextGate->_faninList[i].gate() == this){
                                if(nextGate->_faninList[i].isInv()){
                                    nextGate->_faninList[i].setGate(cirMgr->getGate(0), true);
                                    nextGate->_fanin[i] = 1;
                                }
                                else{
                                    nextGate->_faninList[i].setGate(cirMgr->getGate(0), false);
                                    nextGate->_fanin[i] = 0;
                                }
                            }
                        }

                        break;
                    }
                default:
                    break;
            }
        }
    }
}

void CirGate::dfsStrash(HashMap<HashKey, CirGateV>& hash, UnSet& dfsList, UnSet& deleted){
    UnSet::iterator it = dfsList.find(this->_id);
    if(it == dfsList.end()) return;

    switch(this->getType()){
        case PI_GATE:
            {
                PiGate* piGate = dynamic_cast<PiGate*>(this);
                for(size_t i=0; i<piGate->_fanoutList.size(); ++i)
                    if(!piGate->_fanoutList[i]->isGlobalRef()){
                        piGate->_fanoutList[i]->setToGlobalRef();
                        piGate->_fanoutList[i]->_active = true;
                        piGate->_fanoutList[i]->dfsStrash(hash, dfsList, deleted);
                        piGate->_fanoutList[i]->_active = false;
                    }

                break;
            }
        case CONST_GATE:
            {
                ConstGate* constGate = dynamic_cast<ConstGate*>(this);
                for(size_t i=0; i<constGate->_fanoutList.size(); ++i)
                    if(!constGate->_fanoutList[i]->isGlobalRef()){
                        constGate->_fanoutList[i]->setToGlobalRef();
                        constGate->_fanoutList[i]->_active = true;
                        constGate->_fanoutList[i]->dfsStrash(hash, dfsList, deleted);
                        constGate->_fanoutList[i]->_active = false;
                    }

                break;
            }
        case AIG_GATE:
            {
                AigGate* aigGate = dynamic_cast<AigGate*>(this);
                for(size_t i=0; i<aigGate->_fanoutList.size(); ++i)
                    if(!aigGate->_fanoutList[i]->isGlobalRef()){
                        aigGate->_fanoutList[i]->setToGlobalRef();
                        aigGate->_fanoutList[i]->_active = true;
                        aigGate->_fanoutList[i]->dfsStrash(hash, dfsList, deleted);
                        aigGate->_fanoutList[i]->_active = false;
                    }

                aigGate->simplifyStrash(hash, deleted);

                break;
            }
        default:
            break;
    }

}

void AigGate::simplifyStrash(HashMap<HashKey, CirGateV>& hash, UnSet& deleted){
    HashKey k(this->_fanin[0], this->_fanin[1]);
    CirGateV mergeGate((CirGate*)this);

    if(hash.check(k)){
        CirGateV node(this);
        hash.query(k, node);

        AigGate* mergingGate = this;
        AigGate* gate = dynamic_cast<AigGate*>(node.gate());

        deleted.insert(gate->_id);
        cout << "Strashing: " << mergingGate->_id << " merging " << gate->_id << "..." << endl;
        
        for(int i=0; i<2; ++i){
            switch(gate->_faninList[i].gate()->getType()){
                case UNDEF_GATE:
                    {
                        UndefGate* undefGate = dynamic_cast<UndefGate*>(gate->_faninList[i].gate());
                        for(size_t j=0; j<undefGate->_fanoutList.size(); ++j)
                            if(undefGate->_fanoutList[j] == gate){
                                undefGate->_fanoutList.erase(undefGate->_fanoutList.begin()+j);
                                break;
                            }

                        break;
                    }
                case CONST_GATE:
                    {
                        ConstGate* constGate = dynamic_cast<ConstGate*>(gate->_faninList[i].gate());
                        for(size_t j=0; j<constGate->_fanoutList.size(); ++j)
                            if(constGate->_fanoutList[j] == gate){
                                constGate->_fanoutList.erase(constGate->_fanoutList.begin()+j);
                                break;
                            }

                        break;
                    }
                case PI_GATE:
                    {
                        PiGate* piGate = dynamic_cast<PiGate*>(gate->_faninList[i].gate());
                        for(size_t j=0; j<piGate->_fanoutList.size(); ++j)
                            if(piGate->_fanoutList[j] == gate){
                                piGate->_fanoutList.erase(piGate->_fanoutList.begin()+j);
                                break;
                            }

                        break;
                    }
                case AIG_GATE:
                    {
                        AigGate* aigGate = dynamic_cast<AigGate*>(gate->_faninList[i].gate());
                        for(size_t j=0; j<aigGate->_fanoutList.size(); ++j)
                            if(aigGate->_fanoutList[j] == gate){
                                aigGate->_fanoutList.erase(aigGate->_fanoutList.begin()+j);
                                break;
                            }

                        break;
                    }
                default:
                    break;
            }
        }

        
        for(size_t i=0; i<gate->_fanoutList.size(); ++i){
            mergingGate->_fanoutList.push_back(gate->_fanoutList[i]);

            switch(gate->_fanoutList[i]->getType()){
                case AIG_GATE:
                    {
                        AigGate* aigGate = dynamic_cast<AigGate*>(gate->_fanoutList[i]);
                        for(int j=0; j<2; ++j)
                            if(aigGate->_faninList[j].gate() == gate){
                                if(aigGate->_faninList[j].isInv()){
                                    aigGate->_faninList[j].setGate((CirGate*)mergingGate, true);
                                    aigGate->_fanin[j] = 2*mergingGate->_id+1;
                                }
                                else{
                                    aigGate->_faninList[j].setGate((CirGate*)mergingGate, false);
                                    aigGate->_fanin[j] = 2*mergingGate->_id;
                                }
                                HashKey aigK(aigGate->_fanin[0], aigGate->_fanin[1]);
                                aigGate->simplifyStrash(hash, deleted);
                            }

                        break;
                    }
                case PO_GATE:
                    {
                        PoGate* poGate = dynamic_cast<PoGate*>(gate->_fanoutList[i]);
                        if(poGate->_faninList[0].isInv()){
                            poGate->_faninList[0].setGate((CirGate*)mergingGate, true);
                            poGate->_fanin[0] = 2*mergingGate->_id+1;
                        }
                        else{
                            poGate->_faninList[0].setGate((CirGate*)mergingGate, false);
                            poGate->_fanin[0] = 2*mergingGate->_id;
                        }

                        break;
                    }
                default:
                    break;
            }
        }

        hash.update(k, mergeGate);
    }
    else hash.insert(k, mergeGate);
}
