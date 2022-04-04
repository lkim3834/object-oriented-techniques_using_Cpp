// $Id: debug.cpp,v 1.2 2021-05-18 01:32:29-07 - - $

#include <climits>
#include <iostream>
#include <vector>

using namespace std;

#include "debug.h"

debugflags::flagset debugflags::flags {};

void debugflags::setflags (const string& initflags) {
   for (const unsigned char flag: initflags) {
      if (flag == '@') flags.set();
                  else flags.set (flag, true);
   }
}

// getflag -
//    Check to see if a certain flag is on.

bool debugflags::getflag (char flag) {
   // WARNING: Don't TRACE this function or the stack will blow up.
   return flags.test (static_cast<unsigned char> (flag));
}

void debugflags::where (char flag, const char* file, int line,
                        const char* pretty_function) {
   cerr << "DEBUG(" << flag << ") " << file << "[" << line << "] " 
        << pretty_function << endl;
}

