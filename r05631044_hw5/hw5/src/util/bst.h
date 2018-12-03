/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <stack>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* p, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0):
        _data(d), _parent(p), _left(l), _right(r) {}
   ~BSTreeNode() {}

   T              _data;
   BSTreeNode<T>* _parent;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree(): _root(0), _size(0) {}
   ~BSTree() { clear(); }

   class iterator {
       friend class BSTree;

    public:
       //iterator(BSTreeNode<T>* n = 0): _node(n) {}
       iterator(const BSTree<T>* t, BSTreeNode<T>* n = 0):
           _tree(t), _node(n) {}
       iterator(const iterator& i): _tree(i._tree), _node(i._node) {}
       ~iterator() {}

       const T& operator * () const{ return _node->_data; }
       T& operator * () { return _node->_data; }

       iterator& operator ++ (){
           *this = findNext(this->_node);
           return *this;
       }
       iterator operator ++ (int){
           iterator tmp(*this);

           *this = findNext(this->_node);
           return tmp;
       }
       iterator& operator -- () {
           *this = findPrev(this->_node);
           return *this;
       }
       iterator operator -- (int){
           iterator tmp(*this);

           *this = findPrev(this->_node);
           return tmp;
       }
       iterator& operator = (const iterator& i) { _node = i._node; return *this; }

       bool operator != (const iterator& i) const { return _node != i._node; }
       bool operator == (const iterator& i) const { return _node == i._node; }

    private:
       const BSTree<T>* _tree;
       BSTreeNode<T>* _node;

       // helper functions
       iterator findMin(BSTreeNode<T>* n){
           if(!n->_left){
               this->_node = n;
               return *this;
           }
           else return findMin(n->_left);
       }

       iterator findNext(BSTreeNode<T>* n){
           if(n == 0) { return *this; }
           if(n == tail()._node) { this->_node = 0; return *this; }
           if(!n->_right){
               if(n->_parent){
                   this->_node = n->_parent;
                   while(this->_node->_data <= n->_data)
                       this->_node = this->_node->_parent;
               }
               else this->_node = 0;
               return *this;
           }
           else return findMin(n->_right);
       }

       iterator findMax(BSTreeNode<T>* n){
           if(!n->_right){
               this->_node = n;
               return *this;
           }
           else return findMax(n->_right);
       }

       iterator findPrev(BSTreeNode<T>* n){
           if(n == 0) { return tail(); }
           if(n == head()._node) { return *this; }
           if(!n->_left){
               if(n->_parent){
                   this->_node = this->_node->_parent;
                   while(this->_node->_data > n->_data)
                       this->_node = this->_node->_parent;
               }
               else this->_node = 0;
               return *this;
           }
           else return findMax(n->_left);
       }

       iterator tail(){
           iterator it(*this);
           it._node = _tree->_root;
           while(it._node->_right)
               it._node = it._node->_right;
           return it;
       }

       iterator head(){
           iterator it(_tree->begin());
           return it;
       }

   };

   iterator begin() const{
       if(empty()) return end();

       iterator it(const_cast<BSTree<T>*>(this), _root);
       while(it._node->_left)
           it._node = it._node->_left;

       return it;
   }

   iterator end() const{
       iterator it(const_cast<BSTree<T>*>(this));
       return it;
   }

   bool empty() const { return _root == 0; }

   size_t size() const { return _size; }

   void insert(const T& x){
       if(empty()){
           _root = new BSTreeNode<T>(x, 0);
           ++_size;
           return;
       }

       iterator it(const_cast<BSTree<T>*>(this), _root);
       while(true){
           if(x == *it){
               if(it._node->_right){
                   it._node->_right = new BSTreeNode<T>(x, it._node, 0, it._node->_right);
                   it._node->_right->_right->_parent = it._node->_right;
               }
               else
                   it._node->_right = new BSTreeNode<T>(x, it._node);
               break;
           }

           else if(x < *it){
               if(it._node->_left)
                   it._node = it._node->_left;
               else{
                   it._node->_left = new BSTreeNode<T>(x, it._node);
                   break;
               }
           }

           else{
               if(it._node->_right)
                   it._node = it._node->_right;
               else{
                   it._node->_right = new BSTreeNode<T>(x, it._node);
                   break;
               }
           }
       }

       ++_size;
   }

   void pop_front(){
       if(empty()) {}
       else erase(begin());
   }

   void pop_back(){
       if(empty()) {}
       else erase(--end());
   }

   bool erase(iterator pos){
       if(empty()) return false;
       if(pos._node == 0) return false;
       
       if(isExternal(pos._node)){
           if(pos._node == _root)
               _root = 0;
           else{
               if(pos._node->_parent->_left){
                   if(pos._node->_parent->_left->_data == *pos)
                       pos._node->_parent->_left = 0;
               }
               if(pos._node->_parent->_right){
                   if(pos._node->_parent->_right->_data == *pos)
                       pos._node->_parent->_right = 0;
               }
           }
       }

       else if(pos._node->_left && !pos._node->_right){
           if(pos._node == _root){
               _root = pos._node->_left;
               _root->_parent = 0;
           }
           else{
               if(pos._node->_parent->_left){
                   if(pos._node->_parent->_left->_data == *pos){
                       pos._node->_parent->_left = pos._node->_left;
                       pos._node->_left->_parent = pos._node->_parent;
                   }
               }
               if(pos._node->_parent->_right){
                   if(pos._node->_parent->_right->_data == *pos){
                       pos._node->_parent->_right = pos._node->_left;
                       pos._node->_left->_parent = pos._node->_parent;
                   }
               }
           }
       }

       else if(pos._node->_right && !pos._node->_left){
           if(pos._node == _root){
               _root = pos._node->_right;
               _root->_parent = 0;
           }
           else{
               if(pos._node->_parent->_left){
                   if(pos._node->_parent->_left->_data == *pos){
                       pos._node->_parent->_left = pos._node->_right;
                       pos._node->_right->_parent = pos._node->_parent;
                   }
               }
               if(pos._node->_parent->_right){
                   if(pos._node->_parent->_right->_data == *pos){
                       pos._node->_parent->_right = pos._node->_right;
                       pos._node->_right->_parent = pos._node->_parent;
                   }
               }
           }
       }

       else{
           if(pos._node == _root){
               iterator it(const_cast<BSTree<T>*>(this), _root);
               _root = (++it)._node;

               if(_root == pos._node->_right){
                   _root->_left = pos._node->_left;
                   pos._node->_left->_parent = _root;
                   _root->_parent = 0;
               }
               else{
                   if(_root->_parent->_left){
                       if(_root->_parent->_left->_data == _root->_data)
                           _root->_parent->_left = 0;
                   }
                   if(_root->_parent->_right){
                       if(_root->_parent->_right->_data == _root->_data)
                           _root->_parent->_right = 0;
                   }

                   _root->_left = pos._node->_left;
                   pos._node->_left->_parent = _root;
                   _root->_right = pos._node->_right;
                   pos._node->_right->_parent = _root;
                   _root->_parent = 0;
               }
           }
           else{
               iterator it(pos);
               iterator itSucc(++it);
               if(itSucc._node->_parent->_left){
                   if(itSucc._node->_parent->_left->_data == *itSucc)
                       itSucc._node->_parent->_left = 0;
               }
               if(itSucc._node->_parent->_right){
                   if(itSucc._node->_parent->_right->_data == *itSucc)
                       itSucc._node->_parent->_right = 0;
               }

               if(pos._node->_parent->_left){
                   if(pos._node->_parent->_left->_data == *pos){
                       pos._node->_parent->_left = itSucc._node;
                       itSucc._node->_parent = pos._node->_parent;
                   }
               }
               if(pos._node->_parent->_right){
                   if(pos._node->_parent->_right->_data == *pos){
                       pos._node->_parent->_right = itSucc._node;
                       itSucc._node->_parent = pos._node->_parent;
                   }
               }

               if(pos._node->_left){
                   itSucc._node->_left = pos._node->_left;
                   pos._node->_left->_parent = itSucc._node;
               }
               if(pos._node->_right){
                   itSucc._node->_right = pos._node->_right;
                   pos._node->_right->_parent = itSucc._node;
               }
           }
       }
       
       delete pos._node;
       --_size;
       return true;
   }

   bool erase(const T& x){
       if(empty()) return false;

       iterator it = find(x);
       if(it != end()){
           erase(it);
           return true;
       }
       return false;
   }

   iterator find(const T& x){
       if(empty()) return end();

       iterator it(const_cast<BSTree<T>*>(this), _root);
       while(true){
           if(x == *it) return it;

           if(x < *it){
               if(!it._node->_left) return end();
               else it._node = it._node->_left;
           }

           else{
               if(!it._node->_right) return end();
               else it._node = it._node->_right;
           }
       }
   }

   void clear() {
       while(!empty()) pop_back();
       _size = 0;
       _root = 0;
   }

   void sort() const {}

   void print() const {}

   // helper functions
   bool isExternal(BSTreeNode<T>* p){
       return p->_left == 0 && p->_right == 0;
   }
   
private:
   BSTreeNode<T>* _root;
   size_t _size;
};

#endif // BST_H
