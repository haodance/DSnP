/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(HashSet* h, Data* d = 0, size_t i = 0, size_t j = 0): 
          _hash(h), _data(d), _i(i), _j(j) {}
      iterator(const iterator& i): 
          _hash(i._hash), _data(i._data), _i(i._i), _j(i._j) {}
      ~iterator() {}

      const Data& operator * () const { return *_data; }
      iterator& operator ++ () {
          if(_j+1 < _hash->_buckets[_i].size())
              _data = &_hash->_buckets[_i][++_j];
          else{
              if(_i+1 >= _hash->_numBuckets){ _data = 0; _i = _j = 0; return *this; }
              _j = 0;
              while(_hash->_buckets[++_i].empty())
                  if(_i >= _hash->_numBuckets-1) { _data = 0; _i = 0; return *this; }
              _data = &_hash->_buckets[_i][_j];
          }
          return *this;
      }
      iterator operator ++ (int) {
          iterator tmp(*this);
          if(_j+1 < _hash->_buckets[_i].size())
              _data = &_hash->_buckets[_i][++_j];
          else{
              if(_i+1 >= _hash->_numBuckets){ _data = 0; _i = _j = 0; return *this; }
              _j = 0;
              while(_hash->_buckets[++_i].empty())
                  if(_i >= _hash->_numBuckets-1) { _data = 0; _i = 0; return *this; }
              _data = &_hash->_buckets[_i][_j];
          }
          return tmp;
      }
      iterator& operator -- () {
          if(_j > 0)
              _data = &_hash->_buckets[_i][--_j];
          else{
              if(_i == 0) { _data = 0; _i = _j = 0; return *this; }
              while(_hash->_buckets[--_i].empty())
                  if(_i == 0) { _data = 0; _i = _j = 0; return *this; }
              _j = _buckets[_i].size()-1;
              _data = &_hash->_buckets[_i][_j];
          }
          return *this;
      }
      iterator operator --(int) {
          iterator tmp(*this);
          if(_j > 0)
              _data = &_hash->_buckets[_i][--_j];
          else{
              if(_i == 0) { _data = 0; _i = _j = 0; return *this; }
              while(_hash->_buckets[--_i].empty())
                  if(_i == 0) { _data = 0; _i = _j = 0; return *this; }
              _j = _buckets[_i].size()-1;
              _data = &_hash->_buckets[_i][_j];
          }
          return tmp;
      }

      iterator& operator = (const iterator& i) { _data = i._data; _i = i._i; _j = i._j; return *this; }

      bool operator != (const iterator& i) const { return _data != i._data; }
      bool operator == (const iterator& i) const { return _data == i._data; }

   private:
      const HashSet* _hash;
      Data* _data;
      size_t _i;
      size_t _j;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   //
   iterator begin() const {
       iterator it(const_cast<HashSet<Data>*>(this));
       for(size_t i=0; i<_numBuckets; ++i)
           if(!_buckets[i].empty())
               return iterator(const_cast<HashSet<Data>*>(this), &_buckets[i][0], i, 0);
       return it;
   }

   // Pass the end
   iterator end() const { return iterator(const_cast<HashSet<Data>*>(this)); }

   // return true if no valid data
   bool empty() const {
       for(size_t i=0; i<_buckets->size(); ++i)
           if(!_buckets[i].empty()) return false;
       return true;
   }

   // number of valid data
   size_t size() const {
       size_t s = 0;
       for(size_t i=0; i<_numBuckets; ++i)
           s += _buckets[i].size();
       return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
       size_t num = bucketNum(d);
       for(size_t i=0; i<_buckets[num].size(); ++i)
           if(_buckets[num][i] == d) return true;
       return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
       size_t num = bucketNum(d);
       for(size_t i=0; i<_buckets[num].size(); ++i)
           if(_buckets[num][i] == d){
               d = _buckets[num][i];
               return true;
           }
       return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
       size_t num = bucketNum(d);
       for(size_t i=0; i<_buckets[num].size(); ++i){
           if(_buckets[num][i] == d){
               if(i == 0){
                   _buckets[num][i] = d;
                   return true;
               }
               Data tmp = _buckets[num][0];
               _buckets[num][0] = d;
               _buckets[num][i] = tmp;
               return true;
           }
       }

       Data tmp = _buckets[num][0];
       _buckets[num][0] = d;
       _buckets[num].push_back(tmp);
       return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
       if(check(d)) return false;
       
       _buckets[bucketNum(d)].push_back(d);
       return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
       size_t num = bucketNum(d);
       for(size_t i=0; i<_buckets[num].size(); ++i)
           if(_buckets[num][i] == d){
               _buckets[num].erase(_buckets[num].begin()+i);
               return true;
           }
       return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
