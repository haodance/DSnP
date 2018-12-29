/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }

   void insert(const Data& d) {
        _data.push_back(d);
        int t = _data.size();
       while(t > 1){
           int p = t / 2;
           if(!(d < _data[p-1]))
               break;

           _data[t-1] = _data[p-1];
           t = p;
       }
       _data[t-1] = d;
   }

   void delMin() {
       int p = 1, t = 2 * p, n = _data.size();
       while(t <= n){
           if(t < n)
               if(_data[t] < _data[t-1]) ++t;
           if(_data[n-1] < _data[t-1])
               break;
           _data[p-1] = _data[t-1];
           p = t;
           t = 2 * p;
       }
       _data[p-1] = _data[n-1];
       _data.erase(--_data.end());
   }

   void delData(size_t i) {
       ++i;
       int t = 2 * i, n = _data.size();
       while(t <= n){
           if(t < n)
               if(_data[t] < _data[t-1]) ++t;
           if(_data[n-1] < _data[t-1])
               break;
           _data[i-1] = _data[t-1];
           i = t;
           t = 2 * i;
       }
       _data[i-1] = _data[n-1];
       _data.erase(--_data.end());
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
