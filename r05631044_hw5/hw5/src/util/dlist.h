/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList(): _isSorted(false) {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next; return *this; }
      iterator operator ++ (int) { iterator tmp = *this; _node = _node->_next; return tmp; }
      iterator& operator -- () { _node = _node->_prev; return *this; }
      iterator operator -- (int) { iterator tmp = *this; _node = _node->_prev; return tmp; }

      iterator& operator = (const iterator& i) { _node = i._node; return *this; }

      bool operator != (const iterator& i) const { return _node != i._node; }
      bool operator == (const iterator& i) const { return _node == i._node; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const {
       iterator it(_head);
       return it;
   }
   iterator end() const {
       iterator it(_head);
       --it;
       return it;
   }
   bool empty() const { return _head->_next==_head && _head->_prev==_head; }
   size_t size() const {
       size_t size = 0;
       iterator it;
       for(it=begin(); it!=end(); ++it)
           ++size;

       return size;
   }

   void push_back(const T& x) {
       iterator it(--end());
       if(empty()){
           it._node->_next = new DListNode<T>(x, it._node, it._node);
           it._node->_prev = it._node->_next;
           _head = it._node->_next;
       }
       else{
           it._node->_next = new DListNode<T>(x, it._node, _head->_prev);
           _head->_prev->_prev = it._node->_next;
       }
       _isSorted = false;
   }
   void pop_front() {
       if(empty()) {}
       else{
           DListNode<T>* tmp = _head;
           _head = _head->_next;
           _head->_prev = tmp->_prev;
           tmp->_prev->_next = _head;
           delete tmp;
       }
   }
   void pop_back() {
       if(empty()) {}
       else{
           DListNode<T>* tmp = (--end())._node;
           iterator itend(end());
           if(tmp == _head){
               _head = _head->_prev;
               _head->_next = _head->_prev = _head;
               _isSorted = false;
           }
           else{
               itend._node->_prev = tmp->_prev;
               tmp->_prev->_next = itend._node;
           }
           delete tmp;
       }
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
       if(empty()) return false;

       if(pos._node == _head) _head = _head->_next;
       pos._node->_prev->_next = pos._node->_next;
       pos._node->_next->_prev = pos._node->_prev;
       delete pos._node;
       pos._node = 0;
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
           if(it._node->_data == x) return it;

       return end();
   }

   void clear() {
       while(!empty()) pop_back();
       _isSorted = false;
   }  // delete all nodes except for the dummy node

   void sort() const {
       if(!empty() && size()>1 && !_isSorted)
           insertionSort(_head);
       _isSorted = true;
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions

   void insertionSort(DListNode<T>* head) const{
       DListNode<T>* cur = head->_next;
       DListNode<T>* pre;
       T tmp;

       for(;cur != head->_prev; cur = cur->_next){
           tmp = cur->_data;
           pre = cur->_prev;

           while(tmp < pre->_data){
               pre->_next->_data = pre->_data;
               pre = pre->_prev;
               if(pre == head->_prev) break;
           }
           pre->_next->_data = tmp;
       }
   }

};

#endif // DLIST_H
