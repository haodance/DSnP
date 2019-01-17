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
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;
class CirGateV;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
    friend class CirMgr;
    friend class AigGate;

public:
   CirGate(): _ref(0), _active(false) {}
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const = 0;
   unsigned getLineNo() const { return _line; }
   virtual bool isAig() const { if(getTypeStr()=="AIG") return true; return false; }

   // Printing functions
   virtual void printGate();
   void reportGate() const;
   void reportFanin(int level);
   void reportFanout(int level);


   //
   bool isGlobalRef() const { return _ref == _globalRef; }
   void setToGlobalRef() { _ref = _globalRef; }
   static void setGlobalRef() { ++_globalRef; }
   void setIdLineCol(const unsigned& i, const unsigned& l, const unsigned& c){
       _id = i; _line = l; _col = c;
   }
   virtual GateType getType() const = 0;
   void dfsTraversal(GateList&, CirGatePairList&);
   void printFanin(CirGate*, int&, int, UnSet&, CirGate*);
   void printFanout(CirGate*, int&, int, UnSet&, CirGate*);

   void dfsOptimize(UnSet&);
   void dfsStrash(HashMap<HashKey, CirGateV>&, UnSet&, UnSet&);

private:
   static unsigned _globalRef;
   unsigned _ref;

protected:
   bool _active;
   string _symbol;

   unsigned _id;
   unsigned _line;
   unsigned _col;
};


class CirGateV{
#define NEG 0x1
    public:
        CirGateV(CirGate* g, size_t phase = 0): _gateV(size_t(g)+phase) {}
        ~CirGateV() {}

        CirGate* gate() const { return (CirGate*)(_gateV & ~size_t(NEG)); }
        bool isInv() const { return (_gateV & NEG); }
        void setGate(CirGate* gate, bool inv){
            if(inv) _gateV = (size_t(gate) + 0x1);
            else _gateV = (size_t(gate) + 0x0);
        }
        bool operator == (const CirGateV& c) const { return _gateV == c._gateV; }

    private:
        size_t _gateV;
};


class UndefGate : public CirGate{
    friend class CirMgr;
    friend class CirGate;
    friend class AigGate;

    public:
        UndefGate() {}
        ~UndefGate() {}
        
        string getTypeStr() const { return "UNDEF"; }
        GateType getType() const { return GateType(UNDEF_GATE); }

    private:
        GateList _fanoutList;
};

class PiGate : public CirGate{
    friend class CirMgr;
    friend class CirGate;
    friend class AigGate;

    public:
        PiGate() {}
        ~PiGate() {}

        string getTypeStr() const { return "PI"; }
        GateType getType() const { return GateType(PI_GATE); }

    private:
        GateList _fanoutList;
};

class PoGate : public CirGate{
    friend class CirMgr;
    friend class CirGate;
    friend class AigGate;

    public:
        PoGate() {}
        ~PoGate() {}

        string getTypeStr() const { return "PO"; }
        GateType getType() const { return GateType(PO_GATE); }

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
        GateType getType() const { return GateType(AIG_GATE); }
        void simplifyConst(UnSet&);
        void simplifyFanin(UnSet&);
        void simplifyStrash(HashMap<HashKey, CirGateV>&, UnSet&);

    private:
        vector<CirGateV> _faninList;
        GateList _fanoutList;
        IdList _fanin;
};

class ConstGate : public CirGate{
    friend class CirMgr;
    friend class CirGate;
    friend class AigGate;

    public:
        ConstGate() {}
        ~ConstGate() {}

        string getTypeStr() const { return "CONST"; }
        GateType getType() const { return GateType(CONST_GATE); }

    private:
        GateList _fanoutList;
};

#endif // CIR_GATE_H
