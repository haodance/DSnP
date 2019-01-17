/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
// 
//    size_t operator() () const { return 0; }
// 
//    bool operator == (const HashKey& k) const { return true; }
// 
// private:
// };


class HashKey
{
    public:
        HashKey(size_t in0 = 0, size_t in1 = 0): _in0(in0), _in1(in1) {}
        ~HashKey() {}

        size_t operator () () const {
            size_t k = 0;
            size_t n = (_in0 + _in1) / 2;
            for(size_t i=0; i<n; ++i)
                k ^= (n << (i*6));
            return k;
        }
        HashKey& operator = (const HashKey& k) { _in0 = k._in0; _in1 = k._in1; return *this; }
        bool operator == (const HashKey& k) const {
            if((_in0 == k._in0) && (_in1 == k._in1)) return true;
            if((_in1 == k._in0) && (_in0 == k._in1)) return true;
            return false;
        }

    private:
        size_t _in0, _in1;
};


template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
      iterator(HashMap* h, HashNode* n = 0, size_t i = 0, size_t j = 0)
          : _hash(h), _hashNode(n), _i(i), _j(j) {}
      ~iterator() {}

      const HashNode& operator * () { return *_hashNode; }
      iterator& operator = (const iterator& i) { _hashNode = i._hashNode; return *this; }
      bool operator == (const iterator& i) { return _hashNode == i._hashNode; }
      bool operator != (const iterator& i) { return _hashNode != i._hashNode; }

      iterator& operator ++ () {
          if(_j+1 < _hash->_buckets[_i].size())
              _hashNode = &_hash->_buckets[_i][++_j];
          else{
              if(_i+1 >= _hash->_numBuckets) { _hashNode = 0; _i = _j = 0; return *this; }
              _j = 0;
              while(_hash->_buckets[++_i].empty())
                  if(_i >= _hash->_numBuckets-1) { _hashNode = 0; _i = 0; return *this; }
              _hashNode = &_hash->_buckets[_i][_j];
          }
          return *this;
      }

      iterator operator ++ (int) {
          iterator tmp(*this);
          if(_j+1 < _hash->_buckets[_i].size())
              _hashNode = &_hash->_buckets[_i][++_j];
          else{
              if(_i+1 >= _hash->_numBuckets) { _hashNode = 0; _i = _j = 0; return tmp; }
              _j = 0;
              while(_hash->_buckets[++_i].empty())
                  if(_i >= _hash->_numBuckets-1) { _hashNode = 0; _i = 0; return tmp; }
              _hashNode = &_hash->_buckets[_i][_j];
          }
          return tmp;
      }

      iterator& operator -- () {
          if(_j > 0)
              _hashNode = &_hash->_buckets[_i][--_j];
          else{
              if(_i == 0) { _hashNode = 0; _i = _j = 0; return *this; }
              while(_hash->_buckets[--_i].empty())
                  if(_i == 0) { _hashNode = 0; _i = _j = 0; return *this; }
              _j = _buckets[_i].size()-1;
              _hashNode = &_hash->_buckets[_i][_j];
          }
          return *this;
      }

      iterator operator -- (int) {
          iterator tmp(*this);
          if(_j > 0)
              _hashNode = &_hash->_buckets[_i][--_j];
          else{
              if(_i == 0) { _hashNode = 0; _i = _j = 0; return tmp; }
              while(_hash->_buckets[--_i].empty())
                  if(_i == 0) { _hashNode = 0; _i = _j = 0; return tmp; }
              _j = _buckets[_i].size()-1;
              _hashNode = &_hash->_buckets[_i][_j];
          }
          return tmp;
      }

   private:
      const HashMap* _hash;
      HashNode* _hashNode;
      size_t _i, _j;
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
       iterator it(const_cast<HashMap<HashKey, HashData>*>(this));
       for(size_t i=0; i<_numBuckets; ++i)
           if(!_buckets[i].empty())
               return iterator(const_cast<HashMap<HashKey, HashData>*>(this), &_buckets[i][0], i, 0);
       return it;
   }
   // Pass the end
   iterator end() const { return iterator(const_cast<HashMap<HashKey, HashData>*>(this)); }
   // return true if no valid data
   bool empty() const {
       for(size_t i=0; i<_buckets->_size(); ++i)
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

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const {
       size_t num = bucketNum(k);
       for(size_t i=0; i<_buckets[num].size(); ++i)
           if(_buckets[num][i].first == k) return true;
       return false;
   }

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const {
       size_t num = bucketNum(k);
       for(size_t i=0; i<_buckets[num].size(); ++i)
           if(_buckets[num][i].first == k){
               d = _buckets[num][i].second;
               return true;
           }
       return false;
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) {
       size_t num = bucketNum(k);
       for(size_t i=0; i<_buckets[num].size(); ++i)
           if(_buckets[num][i].first == k){
               if(i == 0) { _buckets[num][i].second = d; return true; }
               HashNode tmp = _buckets[num][0];
               _buckets[num][0].first = k;
               _buckets[num][0].second = d;
               _buckets[num][i] = tmp;
               return true;
           }

       HashNode tmp = _buckets[num][0];
       _buckets[num][0].first = k;
       _buckets[num][0].second = d;
       _buckets[num].push_back(tmp);
       return false;
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) {
       if(check(k)) return false;
       HashNode newNode = make_pair(k, d);
       _buckets[bucketNum(k)].push_back(newNode);
       return true;
   }

   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) {
       size_t num = bucketNum(k);
       for(size_t i=0; i<_buckets[num].size(); ++i)
           if(_buckets[num][i].first == k){
               _buckets[num].erase(_buckets[num].erase+i);
               return true;
           }
       return false;
   }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
