// $Id: leakyifkilled.cpp,v 1.2 2021-02-26 15:26:40-08 - - $

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <unistd.h>

int main (int argc, char** argv) {
   vector<string> argvec {&argv[0], &argv[argc]};
   for (size_t count = 0;; ++count) {
      cout << argvec[0] << ": count = " << count << endl;
      sleep (5);
   }
   
}
