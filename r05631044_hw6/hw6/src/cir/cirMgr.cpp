/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include <stack>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

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
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
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
       cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
       return false;
   }

   unsigned count = 0;
   bool comments = false;
   while(fin.getline(buf, 1024)){
       stringstream ss(buf);
       string str;
       vector<string> inputs;
       while(getline(ss, str, ' ')){
           inputs.push_back(str);
       }

       //MILOA
       if(count < 1){
           for(size_t i=1; i<6; ++i){
               int tmp;
               if(myStr2Int(inputs[i], tmp))
                   _MILOA.push_back(tmp);
               //cout << tmp << " ";
           }
           //cout << endl;
           ++count;
       }

       //pi
       else if(count < 1+_MILOA[1]){
           int tmp;
           if(myStr2Int(inputs[0], tmp)){
               //cout << tmp << " - PI " << tmp/2 << "(" << 1+count << "," << 1 << ")"<< endl;
               CirGate* gate = new PiGate;
               gate->setIdLineColumn(tmp/2, 1+count, 1);
               _piList.push_back(gate);
               _totalList.insert(make_pair(gate->_id, gate));
           }
           ++count;
       }

       //po
       else if(count < 1+_MILOA[1]+_MILOA[3]){
           int tmp;
           if(myStr2Int(inputs[0], tmp)){
               //cout << tmp << " - PO " << tmp/2 << "(" << 1+count << "," << 1 << ")" << endl;
               CirGate* gate = new PoGate;
               gate->setIdLineColumn(_MILOA[0]+1+_poList.size(), 1+count, 1);
               dynamic_cast<PoGate*>(gate)->_fanin.push_back(tmp);
               _poList.push_back(gate);
               _totalList.insert(make_pair(gate->_id, gate));
           }
           ++count;
       }

       //aig
       else if(count < 1+_MILOA[1]+_MILOA[3]+_MILOA[4]){
           int tmp1, tmp2, tmp3;
           if(myStr2Int(inputs[0], tmp1)
                && myStr2Int(inputs[1], tmp2)
                && myStr2Int(inputs[2], tmp3)){
               /*cout << tmp1 << " " << tmp2 << " " << tmp3;
               cout << " - aig " << tmp1/2 << " ";
               if(tmp2 % 2 == 0) cout << tmp2/2 << " ";
               else cout << "!" << tmp2/2 << " ";
               if(tmp3 % 2 == 0) cout << tmp3/2 << " ";
               else cout << "!" << tmp3/2 << " ";
               cout << "(" << 1+count << "," << 1 << ")" << endl;*/
               CirGate* gate = new AigGate;
               gate->setIdLineColumn(tmp1/2, 1+count, 1);
               dynamic_cast<AigGate*>(gate)->_fanin.push_back(tmp2);
               dynamic_cast<AigGate*>(gate)->_fanin.push_back(tmp3);
               _totalList.insert(make_pair(gate->_id, gate));
           }
           ++count;
       }

       //symbols
       else if(inputs[0] != "c" && !comments){
           /*
           unsigned letterCount = 1;
           for(size_t i=0; i<inputs.size(); ++i){
               if(i == 0){
                   cout << inputs[i] << " (" << 1+count << "," << 1 << ")" << endl;
                   letterCount += inputs[i].size()+1;
               }
               else{
                   cout << inputs[i] << " (" << 1+count << "," << letterCount << ")" << endl;
                   letterCount += inputs[i].size()+1;
               }
           }
           */
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

       //comments c
       else if(inputs[0] == "c"){
           comments = true;
           //cout << inputs[0] << " (" << 1+count << "," << 1 << ")" << endl;
           _comments.push_back(inputs[0]);
       }

       //comments
       else{
           /*
           for(size_t i=0; i<inputs.size(); ++i)
               cout << inputs[i] << " ";
           cout << endl;
           */
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
    unordered_map<unsigned, CirGate*>::iterator it;
    for(it=_totalList.begin(); it!=_totalList.end(); ++it){
        if(it->second->getTypeStr() != "AIG") continue;

        AigGate* aigGate = dynamic_cast<AigGate*>(it->second);
        for(size_t i=0; i<aigGate->_fanin.size(); ++i){
            CirGate* faninGate = getGate(aigGate->_fanin[i]/2);
            if(faninGate == 0){
                faninGate = new UndefGate;
                faninGate->setIdLineColumn(aigGate->_fanin[i]/2, 0, 0);
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

            if(faninGate->getTypeStr() == "PI")
                dynamic_cast<PiGate*>(faninGate)->_fanoutList.push_back(it->second);
            else if(faninGate->getTypeStr() == "AIG")
                dynamic_cast<AigGate*>(faninGate)->_fanoutList.push_back(it->second);
            else if(faninGate->getTypeStr() == "CONST")
                dynamic_cast<ConstGate*>(faninGate)->_fanoutList.push_back(it->second);
            else if(faninGate->getTypeStr() == "UNDEF")
                dynamic_cast<UndefGate*>(faninGate)->_fanoutList.push_back(it->second);
        }
    }

    for(size_t i=0; i<_poList.size(); ++i){
        PoGate* poGate = dynamic_cast<PoGate*>(_poList[i]);
        CirGate* faninGate = getGate(poGate->_fanin[0]/2);
        if(faninGate == 0){
            faninGate = new UndefGate;
            faninGate->setIdLineColumn(poGate->_fanin[0]/2, 0, 0);
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

        if(faninGate->getTypeStr() == "PI")
            dynamic_cast<PiGate*>(faninGate)->_fanoutList.push_back(_poList[i]);
        else if(faninGate->getTypeStr() == "AIG")
            dynamic_cast<AigGate*>(faninGate)->_fanoutList.push_back(_poList[i]);
        else if(faninGate->getTypeStr() == "CONST")
            dynamic_cast<ConstGate*>(faninGate)->_fanoutList.push_back(_poList[i]);
        else if(faninGate->getTypeStr() == "UNDEF")
            dynamic_cast<UndefGate*>(faninGate)->_fanoutList.push_back(_poList[i]);
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
    unsigned aigCount = 0;
    unordered_map<unsigned, CirGate*>::const_iterator it;
    for(it=_totalList.begin(); it!=_totalList.end(); ++it)
        if(it->second->getTypeStr() == "AIG") ++aigCount;

    cout << "Circuit Statistics" << endl;
    cout << "==================" << endl;
    cout << setw(7) << left << "  PI" << setw(9) << right << _piList.size() << endl;
    cout << setw(7) << left << "  PO" << setw(9) << right << _poList.size() << endl;
    cout << setw(7) << left << "  AIG" << setw(9) << right << aigCount << endl;
    cout << "------------------" << endl;
    cout << setw(7) << left << "  Total" << setw(9) << right << _piList.size()+_poList.size()+aigCount << endl;
}

void
CirMgr::printNetlist() const
{
    for(size_t i=0; i<_dfsList.size(); ++i){
        if(_dfsList[i]->getTypeStr() == "CONST")
            cout << "[" << i << "] " << setw(4) << left << "CONST" << _dfsList[i]->_id << endl;
        else if(_dfsList[i]->getTypeStr() == "PI"){
            cout << "[" << i << "] " << setw(4) << left << "PI" << _dfsList[i]->_id;

            if(!_dfsList[i]->_symbol.empty()) cout << " (" << _dfsList[i]->_symbol << ")" << endl;
            else cout << endl;
        }
        else if(_dfsList[i]->getTypeStr() == "PO"){
            cout << "[" << i << "] " << setw(4) << left << "PO" << _dfsList[i]->_id << " ";
            if(dynamic_cast<PoGate*>(_dfsList[i])->_faninList[0].gate()->getTypeStr() == "UNDEF") cout << "*";
            if(dynamic_cast<PoGate*>(_dfsList[i])->_faninList[0].isInv()) cout << "!";
            cout << dynamic_cast<PoGate*>(_dfsList[i])->_faninList[0].gate()->_id;

            if(!_dfsList[i]->_symbol.empty()) cout << " (" << _dfsList[i]->_symbol << ")" << endl;
            else cout << endl;
        }
        else if(_dfsList[i]->getTypeStr() == "AIG"){
            cout << "[" << i << "] " << setw(4) << left << "AIG" << _dfsList[i]->_id << " ";
            AigGate* aigGate = dynamic_cast<AigGate*>(_dfsList[i]);
            if(aigGate->_faninList[0].gate()->getTypeStr() == "UNDEF") cout << "*";
            if(aigGate->_faninList[0].isInv()) cout << "!";
            cout << aigGate->_faninList[0].gate()->_id << " ";
            if(aigGate->_faninList[1].gate()->getTypeStr() == "UNDEF") cout << "*";
            if(aigGate->_faninList[1].isInv()) cout << "!";
            cout << aigGate->_faninList[1].gate()->_id << endl;
        }
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
    vector<unsigned> floating, notUsed;
    unordered_map<unsigned, CirGate*>::const_iterator it;
    for(it=_totalList.begin(); it!=_totalList.end(); ++it){
        if(it->second->getTypeStr() == "UNDEF") floating.push_back(it->first);
        
        if(it->second->getTypeStr() == "PI")
            if(dynamic_cast<PiGate*>(it->second)->_fanoutList.empty())
                notUsed.push_back(it->first);
        if(it->second->getTypeStr() == "AIG")
            if(dynamic_cast<AigGate*>(it->second)->_fanoutList.empty())
                notUsed.push_back(it->first);
    }

    if(!floating.empty()){
        cout << "Gates with floating fanin(s):";
        for(int i=floating.size()-1; i>=0; --i){
            if(i == 0) cout << " " << floating[i] << endl;
            else cout << " " << floating[i];
        }
    }

    if(!notUsed.empty()){
        cout << "Gates defined but not used  :";
        for(int i=notUsed.size()-1; i>=0; --i){
            if(i == 0) cout << " " << notUsed[i] << endl;
            else cout << " " << notUsed[i];
        }
    }
}

void
CirMgr::writeAag(ostream& outfile) const
{
    //MILOA
    outfile << "aag";
    for(int i=0; i<5; ++i) outfile << " " << _MILOA[i];
    outfile << endl;

    //PI
    for(size_t i=0; i<_piList.size(); ++i)
        outfile << _piList[i]->_id*2 << endl;

    //PO
    for(size_t i=0; i<_poList.size(); ++i)
        outfile << dynamic_cast<PoGate*>(_poList[i])->_fanin[0] << endl;

    //AIG
    stack<CirGate*> aig;
    unordered_map<unsigned, CirGate*>::const_iterator it;
    for(it=_totalList.begin(); it!=_totalList.end(); ++it)
        if(it->second->getTypeStr() == "AIG"){
            aig.push(it->second);
        }
    while(!aig.empty()){
        outfile << aig.top()->_id*2;
        outfile << " " << dynamic_cast<AigGate*>(aig.top())->_fanin[0];
        outfile << " " << dynamic_cast<AigGate*>(aig.top())->_fanin[1] << endl;
        aig.pop();
    }

    //symbol
    for(size_t i=0; i<_piList.size(); ++i)
        if(!_piList[i]->_symbol.empty())
            outfile << "i" << i << " " << _piList[i]->_symbol << endl;
    for(size_t i=0; i<_poList.size(); ++i)
        if(!_poList[i]->_symbol.empty())
            outfile << "o" << i << " " << _poList[i]->_symbol << endl;

    //comments
    for(size_t i=0; i<_comments.size(); ++i)
        outfile << _comments[i] << endl;
}


/**************************************************************/
/*   Self-defined member functions                            */
/**************************************************************/

void CirMgr::clear(){
    unordered_map<unsigned, CirGate*>::iterator it;
    for(it=_totalList.begin(); it!=_totalList.end(); ++it){
        delete it->second;
        it->second = 0;
    }
}
