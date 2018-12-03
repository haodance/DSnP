/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0),_isSorted(false) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { _node += 1; return *this; }
      iterator operator ++ (int) { iterator tmp = *this; _node += 1; return tmp; }
      iterator& operator -- () { _node -= 1; return *this; }
      iterator operator -- (int) { iterator tmp = *this; _node -= 1; return tmp; }

      iterator operator + (int i) const { iterator tmp = *this; tmp._node += i; return tmp; }
      iterator& operator += (int i) { _node += i; return *this; }

      iterator& operator = (const iterator& i) { _node = i._node; return (*this); }

      bool operator != (const iterator& i) const { return _node != i._node; }
      bool operator == (const iterator& i) const { return _node == i._node; }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { 
       iterator it(_data);
       return it;
   }
   iterator end() const {
       iterator it(_data);
       it += _size;
       return it;
   }
   bool empty() const { return _size == 0; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) {
       if(_size >= _capacity){
           T* tmp = _data;
           
           if(_capacity == 0) ++_capacity;
           else _capacity *= 2;
           _data = new T[_capacity];
           for(int i=0; i<(int)_size; ++i)
               _data[i] = tmp[i];
           delete []tmp;

           _data[_size++] = x;
       }
       else{
           _data[_size++] = x;
       }
       _isSorted = false;
   }
   void pop_front() {
       if(empty()) {}
       else{
           if(_size >= 2)
               _data[0] = _data[_size-1];
           --_size;
           _isSorted = false;
       }
   }
   void pop_back() {
       if(empty()) {}
       else
           --_size;
   }

   bool erase(iterator pos) {
       if(empty()) return false;

       *pos = *--end();
       --_size;
       _isSorted = false;
       return true;
   }
   bool erase(const T& x) {
       if(empty()) return false;

       iterator it = find(x);
       if(it != end()){
           erase(it);
           return true;
       }
       return false;
   }
   
   iterator find(const T& x) {
       iterator it;
       for(it=begin(); it!=end(); ++it)
           if(*it == x) return it;

       return end();
   }

   void clear() { _size = 0; _isSorted = false; }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty() && !_isSorted) ::sort(_data, _data+_size); _isSorted = true; }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
