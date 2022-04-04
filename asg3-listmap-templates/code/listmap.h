// $Id: listmap.h,v 1.28 2021-02-22 12:17:11-08 - - $

#ifndef __LISTMAP_H__
#define __LISTMAP_H__

#include "debug.h"
#include "xless.h"
#include "xpair.h"

#define SHOW_LINK(FLAG,PTR) { \
           DEBUGB (FLAG, #PTR << "=" << PTR \
                   << ": next=" << PTR->next \
                   << ", prev=" << PTR->prev); \
        }

template <typename key_t, typename mapped_t, class less_t=xless<key_t>>
class listmap {
   public:
      using key_type = key_t;
      using mapped_type = mapped_t;
      using value_type = xpair<const key_type, mapped_type>;
   private:
      less_t less;
      struct node;
      struct link {
         node* next{};
         node* prev{};
         link (node* next_, node* prev_): next(next_), prev(prev_){}
      };
      struct node: link {
         value_type value{};
         node (node* next_, node* prev_, const value_type& value_):
               link (next_, prev_), value(value_){}
      };
      // force base case to be derived class
      // get to first or last of the node
      // don't delete on anchor
       
      node* anchor() { return static_cast<node*> (&anchor_); }
      link anchor_ {anchor(), anchor()};
   public:
      class iterator;
      listmap(){};
      listmap (const listmap&);
      listmap& operator= (const listmap&);
      ~listmap();
      iterator insert (const value_type&);
      iterator find (const key_type&);
      iterator erase (iterator position);
      iterator begin() { return anchor()->next; }
      iterator end() { return anchor(); }
      bool empty() const { return anchor_.next == &anchor_; }
      operator bool() const { return not empty(); }
};


template <typename key_t, typename mapped_t, class less_t>
class listmap<key_t,mapped_t,less_t>::iterator {
   friend class listmap<key_t,mapped_t,less_t>;
   private:
      listmap<key_t,mapped_t,less_t>::node* where {nullptr};
      iterator (node* where_): where(where_){};
   public:
      iterator() {}
      value_type& operator*() {
         SHOW_LINK ('b', where);
         return where->value;
      }
      value_type* operator->() { return &(where->value); }
      iterator& operator++() { where = where->next; return *this; }
      iterator& operator--() { where = where->prev; return *this; }
      bool operator== (const iterator& that) const {
         return this->where == that.where;
      }
      bool operator!= (const iterator& that) const {
         return this->where != that.where;
      }
      operator bool() const { return where != nullptr; }
};

#include "listmap.tcc"
#endif

