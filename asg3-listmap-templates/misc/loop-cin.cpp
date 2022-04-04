// $Id: loop-cin.cpp,v 1.3 2019-07-25 11:45:41-07 - - $

#include <iostream>
#include <string>
using namespace std;

int main (int argc, char**) {
   for (int i = 1; i < argc; ++i) {
      string line;
      getline (cin, line);
      if (cin.eof()) {
         cout << "cin.eof() " << i << endl;
      }else {
         cout << "cin(" << i << "): " << line << endl;
      }
   }
}
