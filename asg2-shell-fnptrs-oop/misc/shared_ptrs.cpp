// $Id: shared_ptrs.cpp,v 1.48 2021-04-10 14:21:53-07 - - $

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
using namespace std;

// Illustrate use of shared pointers and copying.

const string indent (5, ' ');

#define SHOWBOX { \
           cout << indent << __PRETTY_FUNCTION__ << ": " \
                << to_string() << endl; \
        }

class sbox {
   friend ostream& operator<< (ostream&, const sbox&);
   private:
      string value;
   public:
      sbox(): value ("<EMPTY>") {SHOWBOX}
      sbox (const sbox& that): value(that.value) {SHOWBOX}
      sbox (sbox&& that): value(that.value) {SHOWBOX}
      sbox& operator= (const sbox& that) {
         if (this != &that) value = that.value;
         SHOWBOX;
         return *this;
      }
      sbox& operator= (sbox&& that) {
         if (this != &that) value = that.value;
         SHOWBOX;
         return *this;
      }
      ~sbox() {SHOWBOX}
      sbox (const string& val): value(val) {SHOWBOX}
      const string& operator*() const { return value; }
      const string to_string() const {
         ostringstream result;
         result << "sbox(" << this << "->\"" << value << "\")";
         return result.str();
      }
};

ostream& operator<< (ostream& out, const sbox& box) {
   return out << "sbox(\"" << box.value << "\")";
}

using sboxptr = shared_ptr<sbox>;

#define LINE "[" << __LINE__ << "] "
#define SHOW(STMT) cout << LINE << #STMT << endl; STMT;

void showptr (const sboxptr& ptr) {
   cout << indent << ptr << "(" << ptr.use_count() << ")->"
        << (ptr ? ptr->to_string() : "nullptr") << endl;
}

int main() {
   SHOW( sboxptr junk = make_shared<sbox> (":junk:"); )
   SHOW( junk = nullptr; )
   SHOW( sboxptr a = make_shared<sbox>(); )
   SHOW( showptr(a); )
   SHOW( sboxptr b = make_shared<sbox> ("foobar"); )
   SHOW( auto single = make_shared<sbox> ("single"); )
   SHOW( showptr(single); )
   SHOW( showptr(b); )
   SHOW( a = b; )
   SHOW( showptr(a); )
   SHOW( showptr(b); )
   SHOW( sboxptr c = a )
   SHOW( showptr(c); )
   SHOW( b = nullptr; )
   SHOW( showptr(b); )
   SHOW( showptr(a); )
   SHOW( return 0; )
}
