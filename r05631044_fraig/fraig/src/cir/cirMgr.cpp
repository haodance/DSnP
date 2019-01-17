/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <stack>

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
    fstream fin(fileName);
    if(!fin){
        cerr << "Cannot open design\"" << fileName << "\"!!" << endl;
        return false;
    }

    unsigned count = 0;
    bool comments = false;
    while(fin.getline(buf, 1024)){
        if(buf[0] == '\0') continue;
        stringstream ss(buf);
        string str;
        vector<string> inputs;
        while(getline(ss, str, ' '))
            inputs.push_back(str);

        //MILOA
        if(count < 1){
            for(size_t i=1; i<6; ++i){
                int tmp;
                if(myStr2Int(inputs[i], tmp))
                    _MILOA.push_back(tmp);
            }
            ++count;
        }

        //PI
        else if(count < 1+_MILOA[1]){
            int tmp;
            if(myStr2Int(inputs[0], tmp)){
                CirGate* gate = new PiGate;
                gate->setIdLineCol(tmp/2, 1+count, 1);
                _piList.push_back(gate);
                _totalList.insert(make_pair(gate->_id, gate));
            }
            ++count;
        }

        //PO
        else if(count < 1+_MILOA[1]+_MILOA[3]){
            int tmp;
            if(myStr2Int(inputs[0], tmp)){
                CirGate* gate = new PoGate;
                gate->setIdLineCol(_MILOA[0]+1+_poList.size(), 1+count, 1);
                dynamic_cast<PoGate*>(gate)->_fanin.push_back(tmp);
                _poList.push_back(gate);
                _totalList.insert(make_pair(gate->_id, gate));
            }
            ++count;
        }

        //AIG
        else if(count < 1+_MILOA[1]+_MILOA[3]+_MILOA[4]){
            int tmp1, tmp2, tmp3;
            if(myStr2Int(inputs[0],tmp1)
                && myStr2Int(inputs[1], tmp2)
                && myStr2Int(inputs[2], tmp3)){
                CirGate* gate = new AigGate;
                gate->setIdLineCol(tmp1/2, 1+count, 1);
                dynamic_cast<AigGate*>(gate)->_fanin.push_back(tmp2);
                dynamic_cast<AigGate*>(gate)->_fanin.push_back(tmp3);
                _totalList.insert(make_pair(gate->_id, gate));
            }
            ++count;
        }

        //Symbols
        else if(inputs[0]!="c" && !comments){
            ++count;
            if(inputs[0][0] == 'i'){
                unsigned id = stoi(inputs[0].substr(1, inputs[0].size()-1));
                string str;
                for(size_t i=1; i<inputs.size(); ++i){
                    if(i == 1) str += inputs[i];
                    else str += " "+inputs[i];
                }
                _piList[id]->_symbol = str;
            }
            else if(inputs[0][0] == 'o'){
                unsigned id = stoi(inputs[0].substr(1, inputs[0].size()-1));
                string str;
                for(size_t i=1; i<inputs.size(); ++i){
                    if(i == 1) str += inputs[i];
                    else str += " "+inputs[i];
                }
                _poList[id]->_symbol = str;
            }
        }

        //C
        else if(inputs[0] == "c"){
            comments = true;
            _comments.push_back(inputs[0]);
        }

        //Comments
        else{
            string str;
            for(size_t i=0; i<inputs.size(); ++i){
                if(i == 0) str += inputs[i];
                else str += " "+inputs[i];
            }
            _comments.push_back(str);
        }
    }

    constructNet();
    dfsTraversal(_poList);

    return true;
}

void CirMgr::constructNet(){
    UnMap::iterator it;
    for(it=_totalList.begin(); it!=_totalList.end(); ++it){
        if(it->second->getTypeStr() != "AIG") continue;

        AigGate* aigGate = dynamic_cast<AigGate*>(it->second);
        for(int i=0; i<2; ++i){
            CirGate* faninGate = getGate(aigGate->_fanin[i]/2);
            if(faninGate == 0){
                faninGate = new UndefGate;
                faninGate->setIdLineCol(aigGate->_fanin[i]/2, 0, 0);
                _totalList.insert(make_pair(faninGate->_id, faninGate));
            }

            if(aigGate->_fanin[i] % 2){
                CirGateV gate(faninGate, 0x1);
                aigGate->_faninList.push_back(gate);
            }
            else{
                CirGateV gate(faninGate, 0x0);
                aigGate->_faninList.push_back(gate);
            }

            switch(faninGate->getType()){
                case PO_GATE:
                    break;
                case PI_GATE:
                    dynamic_cast<PiGate*>(faninGate)->_fanoutList.push_back(it->second);
                    break;
                case AIG_GATE:
                    dynamic_cast<AigGate*>(faninGate)->_fanoutList.push_back(it->second);
                    break;
                case CONST_GATE:
                    dynamic_cast<ConstGate*>(faninGate)->_fanoutList.push_back(it->second);
                    break;
                case UNDEF_GATE:
                    dynamic_cast<UndefGate*>(faninGate)->_fanoutList.push_back(it->second);
                    break;
                default:
                    break;
            }
        }
    }

    for(size_t i=0; i<_poList.size(); ++i){
        PoGate* poGate = dynamic_cast<PoGate*>(_poList[i]);
        CirGate* faninGate = getGate(poGate->_fanin[0]/2);
        if(faninGate == 0){
            faninGate = new UndefGate;
            faninGate->setIdLineCol(poGate->_fanin[0]/2, 0, 0);
            _totalList.insert(make_pair(faninGate->_id, faninGate));
        }

        if(poGate->_fanin[0] % 2){
            CirGateV gate(faninGate, 0x1);
            poGate->_faninList.push_back(gate);
        }
        else{
            CirGateV gate(faninGate, 0x0);
            poGate->_faninList.push_back(gate);
        }

        switch(faninGate->getType()){
            case PO_GATE:
                break;
            case PI_GATE:
                dynamic_cast<PiGate*>(faninGate)->_fanoutList.push_back(_poList[i]);
                break;
            case AIG_GATE:
                dynamic_cast<AigGate*>(faninGate)->_fanoutList.push_back(_poList[i]);
                break;
            case CONST_GATE:
                dynamic_cast<ConstGate*>(faninGate)->_fanoutList.push_back(_poList[i]);
                break;
            case UNDEF_GATE:
                dynamic_cast<UndefGate*>(faninGate)->_fanoutList.push_back(_poList[i]);
                break;
            default:
                break;
        }
    }
}

void CirMgr::dfsTraversal(const GateList& sinkList){
    CirGate::setGlobalRef();
    for(size_t i=0; i<sinkList.size(); ++i)
        sinkList[i]->dfsTraversal(_dfsList, _fbList);
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
    cout << endl;
    unsigned aigCount = 0;
    UnMap::const_iterator it;
    for(it=_totalList.begin(); it!=_totalList.end(); ++it)
        if(it->second->getTypeStr() == "AIG") ++aigCount;

    cout << "Circuit Statistics" << endl;
    cout << "==================" << endl;
    cout << setw(7) << left << "  PI" << setw(9) << right << _piList.size() << endl;
    cout << setw(7) << left << "  PO" << setw(9) << right << _poList.size() << endl;
    cout << setw(7) << left << "  AIG" << setw(9) << right << aigCount << endl;
    cout << "------------------" << endl;
    cout << setw(7) << left << "  TOTAL" << setw(9) << right << _piList.size()+_poList.size()+aigCount << endl;
}

void
CirMgr::printNetlist() const
{
   cout << endl;
   for(unsigned i=0, n=_dfsList.size(); i<n; ++i) {
      cout << "[" << i << "] ";
      _dfsList[i]->printGate();
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(size_t i=0; i<_piList.size(); ++i)
       cout << " " << _piList[i]->_id;
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(size_t i=0; i<_poList.size(); ++i)
       cout << " " << _poList[i]->_id;
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
    vector<unsigned> floating, unused;
    UnMap::const_iterator it;
    for(it=_totalList.begin(); it!=_totalList.end(); ++it){
        switch(it->second->getType()){
            case UNDEF_GATE:
                {
                    UndefGate* undefGate = dynamic_cast<UndefGate*>(it->second);
                    if(!undefGate->_fanoutList.empty())
                        for(size_t i=0; i<undefGate->_fanoutList.size(); ++i)
                            floating.push_back(undefGate->_fanoutList[i]->_id);
                    break;
                }
            case PI_GATE:
                {
                    if(dynamic_cast<PiGate*>(it->second)->_fanoutList.empty())
                        unused.push_back(it->first);
                    break;
                }
            case AIG_GATE:
                {
                    if(dynamic_cast<AigGate*>(it->second)->_fanoutList.empty())
                        unused.push_back(it->first);
                    break;
                }
            default:
                break;
        }
    }

    if(!floating.empty()){
        cout << "Gates with floating fanin(s):";
        for(int i=floating.size()-1; i>=0; --i)
            cout << " " << floating[i];
        cout << endl;
    }

    if(!unused.empty()){
        cout << "Gates defined but not used  :";
        for(int i=unused.size()-1; i>=0; --i)
            cout << " " << unused[i];
        cout << endl;
    }
}

void
CirMgr::printFECPairs() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
    outfile << "aag";
    for(int i=0; i<5; ++i) cout << " " << _MILOA[i];
    outfile << endl;

    for(size_t i=0; i<_piList.size(); ++i)
        outfile << _piList[i]->_id * 2 << endl;

    for(size_t i=0; i<_poList.size(); ++i)
        outfile << dynamic_cast<PoGate*>(_poList[i])->_fanin[0] << endl;

    stack<CirGate*> aigReverse;
    UnMap::const_iterator it;
    for(it=_totalList.begin(); it!=_totalList.end(); ++it){
        if(it->second->getTypeStr() == "AIG")
            aigReverse.push(it->second);
        while(!aigReverse.empty()){
            outfile << aigReverse.top()->_id * 2;
            outfile << dynamic_cast<AigGate*>(aigReverse.top())->_fanin[0];
            outfile << dynamic_cast<AigGate*>(aigReverse.top())->_fanin[1];
            aigReverse.pop();
        }
    }

    for(size_t i=0; i<_piList.size(); ++i)
        if(!_piList[i]->_symbol.empty())
            outfile << "i" << i << " " << _piList[i]->_symbol << endl;

    for(size_t i=0; i<_poList.size(); ++i)
        if(!_poList[i]->_symbol.empty())
            outfile << "i" << i << " " << _poList[i]->_symbol << endl;

    for(size_t i=0; i<_comments.size(); ++i)
        outfile << _comments[i] << endl;
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}

void CirMgr::init(){
    CirGate* gate = new ConstGate;
    gate->setIdLineCol(0, 0, 0);
    _totalList.insert(make_pair(gate->_id, gate));

    _piList.reserve(10000);
    _poList.reserve(10000);
    _totalList.reserve(100000);
    _dfsList.reserve(100000);
}

void CirMgr::clear(){
    UnMap::iterator it;
    for(it=_totalList.begin(); it!=_totalList.end(); ++it){
        delete it->second;
        it->second = 0;
    }
}

