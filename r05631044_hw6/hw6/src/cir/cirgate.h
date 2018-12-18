/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
    friend class CirMgr;

public:
   CirGate(): _ref(0), _active(false) {}
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const = 0;
   unsigned getLineNo() const { return _line; }

   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level);
   void reportFanout(int level);


   // Self-defined member functions
   bool isGlobalRef() const { return _ref == _globalRef; }
   void setToGlobalRef() { _ref = _globalRef; }
   static void setGlobalRef() { ++_globalRef; }
   void setIdLineColumn(const unsigned& id, const unsigned& line, const unsigned& column){
       _id = id; _line = line; _column = column;
   }
   void dfsTraversal(GateList&, vector< pair<CirGate*, CirGate*> >&);
   void printFanin(CirGate*, int&, int, unordered_set<unsigned>&, CirGate*);
   void printFanout(CirGate*, int&, int, unordered_set<unsigned>&, CirGate*);

private:
   static unsigned _globalRef;
   unsigned _ref;

   unsigned _id;
   unsigned _line;
   unsigned _column;

protected:
   bool _active;
   string _symbol;
};

class CirGateV{
#define NEG 0x1
    public:
        CirGateV(CirGate* g, size_t phase):
            _gateV(size_t(g) + phase) {}
        CirGate* gate() const { return (CirGate*)(_gateV & ~size_t(NEG)); }
        bool isInv() const { return (_gateV & NEG); }
    private:
        size_t _gateV;
};


class UndefGate : public CirGate{
    friend class CirMgr;
    friend class CirGate;

    public:
        UndefGate() {}
        ~UndefGate() {}

        string getTypeStr() const { return "UNDEF"; }
        void printGate() const {}
        
    private:
        GateList _fanoutList;
};

class PiGate : public CirGate{
    friend class CirMgr;
    friend class CirGate;

    public:
        PiGate() {}
        ~PiGate() {}

        string getTypeStr() const { return "PI"; }
        void printGate() const {}

    private:
        GateList _fanoutList;
};

class PoGate : public CirGate{
    friend class CirMgr;
    friend class CirGate;

    public:
        PoGate() {}
        ~PoGate() {}

        string getTypeStr() const { return "PO"; }
        void printGate() const {}

    private:
        vector<CirGateV> _faninList;
        IdList _fanin;
};

class AigGate : public CirGate{
    friend class CirMgr;
    friend class CirGate;

    public:
        AigGate() {}
        ~AigGate() {}

        string getTypeStr() const { return "AIG"; }
        void printGate() const {}

    private:
        vector<CirGateV> _faninList;
        GateList _fanoutList;
        IdList _fanin;
};

class ConstGate : public CirGate{
    friend class CirMgr;
    friend class CirGate;

    public:
        ConstGate() {}
        ~ConstGate() {}

        string getTypeStr() const { return "CONST"; }
        void printGate() const {}

    private:
        GateList _fanoutList;
};

#endif // CIR_GATE_H
