/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : /* TODO */ 
                               if(moveBufPtr(_readBufPtr-1)){
                                   deleteChar();
                               }
                               break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: /* TODO */
                               moveBufPtr(_readBufPtr + 1);
                               break;
         case ARROW_LEFT_KEY : /* TODO */ 
                               moveBufPtr(_readBufPtr - 1);
                               break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : /* TODO */
                               {
                               int pos_tab = (_readBufPtr - _readBuf)/sizeof(char);
                               int offset_tab = (_readBufEnd - _readBufPtr)/sizeof(char);
                               int mod = pos_tab % 8;
                               insertChar(' ',8-mod);
                               break;
                               }
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // TODO...
   
   int offsetptr = (_readBufEnd - ptr)/sizeof(char);
   int pos = (_readBufPtr - _readBuf)/sizeof(char);
   int posptr = (ptr - _readBuf)/sizeof(char);

   if(posptr < 0 || offsetptr < 0){
       mybeep();
       return false;
   }
   else{
       if(posptr < pos){
           for(int i=0; i<(pos - posptr); i++)
               cout << "\b";
           cout.flush();
       }
       else if(posptr > pos){
           for(int i=0; i<(posptr - pos); i++)
               cout << _readBuf[pos+i];
           cout.flush();
       }
       _readBufPtr = ptr;
   }

   return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // TODO...
   int offset = (_readBufEnd - _readBufPtr)/sizeof(char);
   int pos = (_readBufPtr - _readBuf)/sizeof(char);
   int len = (_readBufEnd - _readBuf)/sizeof(char);
   
   if(offset <= 0){
       mybeep();
       return false;
   }
   else{
       for(int i=0; i<offset; i++)
           _readBuf[pos+i] = _readBuf[pos+i+1];
       for(int i=0; i<offset; i++)
           cout << _readBuf[pos+i];
       cout << " ";
       cout.flush();
       for(int i=0; i<offset; i++)
           cout << "\b";
       cout.flush();

       _readBufEnd = &_readBuf[len-1];
   }
   return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...
   assert(repeat >= 1);
   int offset = (_readBufEnd - _readBufPtr)/sizeof(char);
   int len = (_readBufEnd - _readBuf)/sizeof(char);
   int pos = (_readBufPtr - _readBuf)/sizeof(char);
   for(int i=0; i<offset+1; i++)
       _readBuf[len+repeat-i] = _readBuf[len-i];
   for(int i=pos; i<(pos+repeat); i++)
       _readBuf[i] = ch;
   for(int i=pos; i<(len+repeat); i++)
       cout << _readBuf[i];
   cout.flush();

   _readBufEnd = &_readBuf[len+repeat];
   _readBufPtr = &_readBuf[pos+repeat];

   for(int i=0; i<offset; i++)
       cout << "\b";
   cout.flush();
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // TODO...
   moveBufPtr(_readBuf);
   int len = (_readBufEnd - _readBuf)/sizeof(char);
   for(int i=0; i<len; i++){
       _readBuf[i] = 0;
       cout << " " ;
   }
   cout.flush();
   for(int i=0; i<len; i++)
       cout << "\b";
   cout.flush();
   _readBufPtr = _readBufEnd = _readBuf;
   *_readBufEnd = 0;
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // TODO...
   if(index < _historyIdx){
       if(_historyIdx == 0) mybeep();
       else{
           if(_historyIdx == (int)_history.size()){
               string str(_readBuf);
               
               _history.push_back(str);
               _tempCmdStored = true;
           }
           if(index < 0) index = 0;

           _historyIdx = index;
           retrieveHistory();
       }
   }
   else if(index > _historyIdx){
       if(_historyIdx >= (int)_history.size()) mybeep();
       else{
           if(index >= (int)_history.size()-1){
               if(_tempCmdStored == true){
                   _history.pop_back();
                   _tempCmdStored = false;
                   index = _history.size();
               }
               else index = _history.size()-1;
           }
          
           _historyIdx = index;
           retrieveHistory();
       }
   }
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   // TODO...
   if(_tempCmdStored == true){
       _history.pop_back();
       _tempCmdStored = false;
   }

   int first, last;
   string str(_readBuf);
   if(str!="\0"){
       string str_2;
       for(int i=0; i<(int)str.length(); i++){
           if(str[i] == ' '){
               if(i == (int)str.length()-1){
                   first = -1;
                   break;
               }
               else continue;
           }
           else{
               first = i;
               break;
           }
       }
       
       for(int i=str.length()-1; i>=0; i--){
           if(str[i] == ' '){
               if(i == 0){
                   last = -1;
                   break;
               }
               else continue;
           }
           else{
               last = i;
               break;
           }
       }
       
       if(first>=0 && last>=0){
           for(int i=first; i<=last; i++)
               str_2 += str[i];
           
           if(!str_2.empty())
               _history.push_back(str_2);
       }
   }

   _historyIdx = _history.size();

}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
