/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   if(!CmdExec::lexOptions(option, options))
       return CMD_EXEC_ERROR;

   if(options.empty())
       return CmdExec::errorOption(CMD_OPT_MISSING, "");

   string numObject;
   int arrayPos = -2, arraySize, numObjects;
   bool doArray = false;
   for(int i=0; i<(int)options.size(); ++i){
       if(i == arrayPos+1) continue;

       if(myStrNCmp("-Array", options[i], 2) == 0){
           doArray = true;
           arrayPos = i;
           if(i == (int)options.size()-1)
               return CmdExec::errorOption(CMD_OPT_MISSING, options[i]);
           if(!myStr2Int(options[i+1], arraySize) || arraySize <= 0)
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i+1]);
       }
       else{
           if(numObject.size())
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
           numObject = options[i];
           if(!myStr2Int(numObject, numObjects) || numObjects <= 0)
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
       }
   }

   // Use try-catch to catch the bad_alloc exception
   try{
       if(doArray)
           mtest.newArrs(numObjects, arraySize);
       else
           mtest.newObjs(numObjects);
   }catch(bad_alloc& err){}

   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   if(!CmdExec::lexOptions(option, options))
       return CMD_EXEC_ERROR;

   if(options.empty())
       return CmdExec::errorOption(CMD_OPT_MISSING, "");

   int id, indexPos = -2, randomPos = -2;
   bool doArray = false, doIndex = false, doRandom = false;
   for(int i=0; i<(int)options.size(); ++i){
       if(i == indexPos + 1 || i == randomPos + 1) continue;

       if(myStrNCmp("-Array", options[i], 2) == 0)
           doArray = true;

       else if(myStrNCmp("-Index", options[i], 2) == 0){
           doIndex = true;
           indexPos = i;
           if(i == (int)options.size()-1)
               return CmdExec::errorOption(CMD_OPT_MISSING, options[i]);
           if(!myStr2Int(options[i+1], id) || id < 0)
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i+1]);
       }

       else if(myStrNCmp("-Random", options[i], 2) == 0){
           doRandom = true;
           randomPos = i;
           if(i == (int)options.size()-1)
               return CmdExec::errorOption(CMD_OPT_MISSING, options[i]);
           if(!myStr2Int(options[i+1], id) || id <= 0)
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i+1]);
       }

       else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
   }


   if(doIndex){
       if(doArray){
           if(id >= (int)mtest.getArrListSize()){
               cerr << "Size of array list (" << mtest.getArrListSize()
                    << ") is <= " << id << "!!"<< endl;
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[indexPos + 1]);
           }
           mtest.deleteArr(id);
       }

       else{
           if(id >= (int)mtest.getObjListSize()){
               cerr << "Size of object list (" << mtest.getObjListSize()
                    << ") is <= " << id << "!!"<< endl;
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[indexPos + 1]);
           }
           mtest.deleteObj(id);
       }
   }

   else if(doRandom){
       RandomNumGen rn;
       if(doArray){
           if(mtest.getArrListSize() == 0){
               cerr << "Size of array list is 0!!" << endl;
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[randomPos]);
           }
           for(int i=0; i<id; ++i)
               mtest.deleteArr(rn(mtest.getArrListSize()-1));
       }

       else{
           if(mtest.getObjListSize() == 0){
               cerr << "Size of object list is 0!!" << endl;
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[randomPos]);
           }
           for(int i=0; i<id; ++i)
               mtest.deleteObj(rn(mtest.getObjListSize()-1));
       }
   }

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


