// $Id: sortedlist.cpp,v 1.15 2021-05-06 17:51:57-07 - - $

//
// List insertion algorithm.
// Insert nodes into a singly-linked list using only operator<
// to form comparisons.  Do not insert elements that already
// exist.
//

#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

template <typename Type>
struct xless {
   bool operator() (const Type& left, const Type& right) const {
      return left < right;
   }
};

template <typename Type>
struct xgreater {
   bool operator() (const Type& left, const Type& right) const {
      return left > right;
   }
};


template <typename item_t, class less_t=xless<item_t>>
struct sorted_list {
   struct node {
      item_t item;
      node* link;
      node (const item_t& item_, node* link_):
                  item(item_),   link(link_) {
      }
   };
   less_t less;
   node* head = nullptr;

   sorted_list() = default; // Needed because default is suppressed.
   sorted_list (const sorted_list&) = delete;
   sorted_list& operator= (const sorted_list&) = delete;
   sorted_list (sorted_list&&) = delete;
   sorted_list& operator= (sorted_list&&) = delete;
   ~sorted_list();

   void insert (const item_t& newitem);
   const item_t& front() const { return head->item; }
   void pop_front();
};

template <typename item_t, class less_t>
sorted_list<item_t,less_t>::~sorted_list() {
   while (head != nullptr) pop_front();
}

template <typename item_t, class less_t>
void sorted_list<item_t,less_t>::insert (const item_t& newitem) {
   node** curr = &head;
   while (*curr != nullptr and less ((*curr)->item, newitem)) {
      curr = &(*curr)->link;
   }
   if (*curr == nullptr or less (newitem, (*curr)->item)) {
      *curr = new node (newitem, *curr);
   }
}

template <typename item_t, class less_t>
void sorted_list<item_t,less_t>::pop_front() {
   if (head == nullptr) throw underflow_error (__PRETTY_FUNCTION__);
   node* old = head;
   head = head->link;
   delete old;
}

template <typename item_t, class less_t=xless<item_t>>
void process (int argc, char** argv, const string& label) {
   sorted_list<item_t,less_t> list;
   for (char** argp = &argv[1]; argp != &argv[argc]; ++argp) {
      cout << label << ": Insert: " << *argp << endl;
      list.insert (*argp);
   }
   cout << endl;
   for (auto itor = list.head; itor != nullptr; itor = itor->link) {
      cout << label << ": Sorted: " << itor->item << endl;
   }
   cout << endl;
}

int main (int argc, char** argv) {
   process<string> (argc, argv, "Default");
   process<string,xgreater<string>> (argc, argv, "Greater");
   return 0;
}

/*
//TEST// alias grind='valgrind --leak-check=full --show-reachable=yes'
//TEST// grind --log-file=sortedlist.out.log \
//TEST//       sortedlist foo bar baz qux zxcvbnm asdfg qwerty \
//TEST//       bar baz foo qwerty hello hello 01234 56789 \
//TEST//       >sortedlist.out 2>&1
//TEST// mkpspdf sortedlist.ps sortedlist.cpp* sortedlist.out*
*/
