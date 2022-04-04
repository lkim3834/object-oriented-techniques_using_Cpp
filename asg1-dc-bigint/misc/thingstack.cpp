// $Id: thingstack.cpp,v 1.22 2018-06-27 16:51:39-07 - - $

#include <iostream>
#include <list>

using namespace std;

#include "iterstack.h"

int serial = 0;

#define PRINT(FUNC) print(FUNC, __LINE__)

struct thing {
   int ser;
   int val;
   explicit thing(int v = int());
   thing (const thing&);
   thing& operator= (const thing&);
   ~thing();
   void print (const char* name, int line);
};

thing::thing(int v): ser(++serial), val(v) {
   PRINT(__PRETTY_FUNCTION__);
}

thing::thing (const thing& that): ser(++serial), val(that.val) {
   PRINT(__PRETTY_FUNCTION__);
}

thing& thing::operator= (const thing& that) {
   if (this != &that) {
      val = that.val;
   }
   PRINT(__PRETTY_FUNCTION__);
   return *this;
}

thing::~thing() {
   PRINT(__PRETTY_FUNCTION__);
}

void thing::print (const char* name, int line) {
   cout << name << "[" << line << "]: " << this << "-> ser="
        << ser << ", val=" << val << endl;
}


#define SCOPE(X) cout << endl << X << " scope " << __LINE__ << endl
   
int main (int, char**) {
   iterstack<thing> stk;
   for (int i = 0; i < 3; ++i) {
      SCOPE("enter");
      thing t(i);
      cout << endl << "stk.push (t);" << endl;
      stk.push (t);
      SCOPE("leave");
   }
   while (not stk.empty()) {
      SCOPE("enter");
      thing t = stk.top();
      t.PRINT("stk.top()");
      cout << endl << "stk.pop();" << endl;
      stk.pop();
      SCOPE("leave");
   }
   return 0;
}

/*
//TEST// valgrind --leak-check=full --show-reachable=yes \
//TEST//       --log-file=thingstack.out.grind \
//TEST//       thingstack >thingstack.out 2>&1
//TEST// mkpspdf thingstack.ps thingstack.cpp* iterstack.h \
//TEST//       thingstack.out*
*/

