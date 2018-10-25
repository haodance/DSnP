/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());

   j.readin = true;

   string key;
   int value;
   string colon;

   while(is >> key){
       if(key == "{" || key ==",") continue;
       else if(key == "}") break;
       is >> colon >> value;
       key = key.assign(key, 1, key.length()-2);

       if(isValidVarName(key)){
           DBJsonElem elem(key, value);
           j._obj.push_back(elem);
       }
   }
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   os << "{" << endl;
   for(int i=0; i<(int)j.size(); i++){
       if(i == (int)j.size()-1) os << "  " << j._obj[i] << endl;
       else os << "  " << j._obj[i] << "," << endl;
   }
   os << "}" << endl;

   os << "Total JSON elements: " << j.size();

   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
   _obj.clear();
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   for(int i=0; i<(int)_obj.size(); i++)
       if(_obj[i]._key == elm._key) return false;

   _obj.push_back(elm);

   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   if(this->empty()) return NAN;
   else{
       float sum;
       
       for(int i=0; i<(int)_obj.size(); i++)
           sum += _obj[i]._value;

       float ave = sum/_obj.size();

       return ave;
   }
   return 0.0;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   if(this->empty()){
       idx = this->size();
       int maxN = INT_MIN;
       return  maxN;
   }
   else{
       int max = _obj[0]._value;
       idx = 0;
       for(int i=0; i<(int)_obj.size(); i++)
           if(_obj[i]._value > max){
               idx = i;
               max = _obj[i]._value;
           }

       return max;
   }
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
   if(this->empty()){
       idx = this->size();
       int minN = INT_MAX;
       return  minN;
   }
   else{
       int min = _obj[0]._value;
       idx = 0;
       for(int i=0; i<(int)_obj.size(); i++)
           if(_obj[i]._value < min){
               idx = i;
               min = _obj[i]._value;
           }

       return min;
   }
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   if(this->empty()){
       int s = 0;
       return s;
   }
   else{
       int sum;
       for(int i=0; i<(int)_obj.size(); i++)
           sum += _obj[i]._value;

       return sum;
   }
}
