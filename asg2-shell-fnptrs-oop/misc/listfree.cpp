// $Id: listfree.cpp,v 1.24 2019-01-10 13:58:49-08 - - $

// Show how to break a cycle in a simple circular list.

#include <algorithm>
#include <iostream>
#include <memory>
using namespace std;

struct node;

using node_ptr = shared_ptr<node>;

struct node {
   int value;
   node_ptr link;
   node (int value_, node_ptr link_): value(value_), link(link_) {}
};

int main (int argc, char** argv) {
   cout << "Command:";
   for_each (&argv[0], &argv[argc], [](char* arg){cout << " " << arg;});
   cout << endl;
   bool break_cycle = argc > 1 and argv[1] == "-f"s;
   node_ptr list = make_shared<node> (1,
                   make_shared<node> (2,
                   make_shared<node> (3, nullptr)));
   list->link->link->link = list;
   cout << "list = " << list << endl;
   for (auto curr = list;;) {
      cout << curr << " -> {" << curr->value << ", " << curr->link
           << "} (use_count " << curr.use_count() << ")" << endl;
      curr = curr->link;
      if (curr == list) break;
   }
   if (break_cycle) list->link = nullptr;
   return 0;
}

//TEST// valgrind listfree -0 >listfree.out-0 2>&1
//TEST// valgrind listfree -f >listfree.out-f 2>&1
//TEST// mkpspdf listfree.ps listfree.cpp* listfree.out-*

