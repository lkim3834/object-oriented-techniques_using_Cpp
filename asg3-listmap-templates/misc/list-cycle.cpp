// $Id: list-cycle.cpp,v 1.19 2021-04-30 18:31:01-07 - - $

#include <iomanip>
#include <iostream>
#include <list>
using namespace std;

int main() {
   list<int> intlist = {11, 22, 33, 44, 55};
   const size_t max_loops = intlist.size() * 4;
   cout << "sizeof (int) = " << sizeof (int) << endl;
   cout << "sizeof (uintptr_t) = " << sizeof (uintptr_t) << endl;
   cout << "sizeof (list<int>) = " << sizeof (list<int>) << endl;
   cout << "sizeof (list<int>::iterator) = "
        << sizeof (list<int>::iterator) << endl;
   cout << endl;
   cout << "&intlist = " << &intlist << endl;
   cout << "&*intlist.begin() = " << &*intlist.begin() << endl;
   cout << "&*intlist.end() = " << &*intlist.end() << endl;
   cout << endl;
   list<int>::iterator  itor = intlist.begin();
   for (size_t count = 0; count < max_loops; ++count, ++itor) {
      cout << setw (2) << count << ": &*itor = " << &*itor
           << " -> " << *itor;
      if (itor == intlist.end()) cout << " (itor == intlist.end())";
      cout << endl;
   }
}

