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

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

unsigned CirGate::_globalRef = 0;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
    cout << "==================================================" << endl;
    cout << "= " << getTypeStr() << "(" << _id << ")";
    if(!_symbol.empty()) cout << "\"" << _symbol << "\"";
    cout << ", line " << _line;
    if(!_symbol.empty())
        cout << setw(50-13-getTypeStr().size()-to_string(_id).size()-to_string(_line).size()-_symbol.size())
            << right << "=" << endl;
    else cout << setw(50-11-getTypeStr().size()-to_string(_id).size()-to_string(_line).size()) << right << "=" << endl;
    cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level)
{
   assert (level >= 0);

   unordered_set<unsigned> uset;
   printFanin(this, level, 0, uset, 0);
}

void
CirGate::reportFanout(int level)
{
   assert (level >= 0);

   unordered_set<unsigned> uset;
   printFanout(this, level, 0, uset, 0);
}

void CirGate::printFanin(CirGate* gate, int& level, int levelCount, unordered_set<unsigned>& uset, CirGate* ogGate){
    if(levelCount > level) return;

    ++levelCount;
    for(int i=0; i<levelCount-1; ++i) cout << "  ";

    unordered_set<unsigned>::iterator it;

    if(gate->getTypeStr()=="UNDEF" || gate->getTypeStr()=="CONST" || gate->getTypeStr()=="PI"){
        if(ogGate == 0) {}
        else if(ogGate->getTypeStr() == "PO"){
            if(dynamic_cast<PoGate*>(ogGate)->_faninList[0].isInv()) cout << "!";
        }
        else if(ogGate->getTypeStr() == "AIG"){
            if(dynamic_cast<AigGate*>(ogGate)->_faninList[0].gate()==gate
                    && dynamic_cast<AigGate*>(ogGate)->_faninList[0].isInv()) cout << "!";
            else if(dynamic_cast<AigGate*>(ogGate)->_faninList[1].gate()==gate
                    && dynamic_cast<AigGate*>(ogGate)->_faninList[1].isInv()) cout << "!";           
        }
        if(ogGate) uset.insert(ogGate->_id);
        cout << gate->getTypeStr() << " " << gate->_id << endl;
    }

    else if(gate->getTypeStr() == "PO"){
        it = uset.find(gate->_id);
        if(it != uset.end())
            cout << "PO" << " " << gate->_id << " (*)" << endl;
        else{
            cout << "PO" << " " << gate->_id << endl;
            if(ogGate) uset.insert(ogGate->_id);
            PoGate* poGate = dynamic_cast<PoGate*>(gate);
            printFanin(poGate->_faninList[0].gate(), level, levelCount, uset, gate);
        }
    }

    else if(gate->getTypeStr() == "AIG"){
        if(ogGate == 0) {}
        else if(ogGate->getTypeStr() == "PO"){
            if(dynamic_cast<PoGate*>(ogGate)->_faninList[0].isInv()) cout << "!";
        }
        else if(ogGate->getTypeStr() == "AIG"){
            if(dynamic_cast<AigGate*>(ogGate)->_faninList[0].gate()==gate
                    && dynamic_cast<AigGate*>(ogGate)->_faninList[0].isInv()) cout << "!";
            else if(dynamic_cast<AigGate*>(ogGate)->_faninList[1].gate()==gate
                    && dynamic_cast<AigGate*>(ogGate)->_faninList[1].isInv()) cout << "!";           
        }

        it = uset.find(gate->_id);
        if(it != uset.end())
            cout << "AIG" << " " << gate->_id << " (*)" << endl;
        else{
            cout << "AIG" << " " << gate->_id << endl;
            if(ogGate) uset.insert(ogGate->_id);
            AigGate* aigGate = dynamic_cast<AigGate*>(gate);
            for(int i=0; i<2; ++i)
                printFanin(aigGate->_faninList[i].gate(), level, levelCount, uset, gate);
        }
    }
}

void CirGate::printFanout(CirGate* gate, int& level, int levelCount, unordered_set<unsigned>& uset, CirGate* ogGate){
    if(levelCount > level) return;

    ++levelCount;
    for(int i=0; i<levelCount-1; ++i) cout << "  ";

    unordered_set<unsigned>::iterator it;

    if(gate->getTypeStr() == "PO"){
        if(ogGate == 0) {}
        else if(dynamic_cast<PoGate*>(gate)->_faninList[0].isInv()) cout << "!";

        if(ogGate) uset.insert(ogGate->_id);
        cout << "PO" << " " << gate->_id << endl;
    }

    else if(gate->getTypeStr() == "AIG"){
        if(ogGate == 0) {}
        else if(dynamic_cast<AigGate*>(gate)->_faninList[0].gate()==ogGate
                && dynamic_cast<AigGate*>(gate)->_faninList[0].isInv()) cout << "!";
        else if(dynamic_cast<AigGate*>(gate)->_faninList[1].gate()==ogGate
                && dynamic_cast<AigGate*>(gate)->_faninList[1].isInv()) cout << "!";

        it = uset.find(gate->_id);
        if(it != uset.end())
            cout << "AIG" << " " << gate->_id << " (*)" << endl;
        else{
            cout << "AIG" << " " << gate->_id << endl;
            if(ogGate) uset.insert(ogGate->_id);
            AigGate* aigGate = dynamic_cast<AigGate*>(gate);
            for(int i=aigGate->_fanoutList.size()-1; i>=0; --i)
                printFanout(aigGate->_fanoutList[i], level, levelCount, uset, gate);
        }
    }

    else if(gate->getTypeStr() == "PI"){
        it = uset.find(gate->_id);
        if(it != uset.end())
            cout << "PI" << " " << gate->_id << " (*)" << endl;
        else{
            cout << "PI" << " " << gate->_id << endl;
            if(ogGate) uset.insert(ogGate->_id);
            PiGate* piGate = dynamic_cast<PiGate*>(gate);
            for(int i=piGate->_fanoutList.size()-1; i>=0; --i)
                printFanout(piGate->_fanoutList[i], level, levelCount, uset, gate);
        }
    }

    else if(gate->getTypeStr() == "CONST"){
        it = uset.find(gate->_id);
        if(it != uset.end())
            cout << "CONST" << " " << gate->_id << " (*)" << endl;
        else{
            cout << "CONST" << " " << gate->_id << endl;
            if(ogGate) uset.insert(ogGate->_id);
            ConstGate* constGate = dynamic_cast<ConstGate*>(gate);
            for(int i=constGate->_fanoutList.size()-1; i>=0; --i)
                printFanout(constGate->_fanoutList[i], level, levelCount, uset, gate);
        }
    }

    else if(gate->getTypeStr() == "UNDEF"){
        it = uset.find(gate->_id);
        if(it != uset.end())
            cout << "UNDEF" << " " << gate->_id << " (*)" << endl;
        else{
            cout << "UNDEF" << " " << gate->_id << endl;
            if(ogGate) uset.insert(ogGate->_id);
            UndefGate* undefGate = dynamic_cast<UndefGate*>(gate);
            for(int i=undefGate->_fanoutList.size()-1; i>=0; --i)
                printFanout(undefGate->_fanoutList[i], level, levelCount, uset, gate);
        }
    }
}

void CirGate::dfsTraversal(GateList& dfsList, vector< pair<CirGate*, CirGate*> >& fbList){
    if(this->getTypeStr() == "UNDEF") return;

    else if(this->getTypeStr() == "PI" || this->getTypeStr() == "CONST")
        dfsList.push_back(this);
    
    else if(this->getTypeStr() == "PO"){
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
    }

     else if(this->getTypeStr() == "AIG"){
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
    }   
}
