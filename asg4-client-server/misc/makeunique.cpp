// $Id: makeunique.cpp,v 1.12 2021-05-20 17:15:09-07 - - $

//
// Example of using a unique_ptr to point at dynamically
// allocated local data.  C++ does not allow local arrays
// to be given a dynamic size, as in "char buffer[n]".
// This example is useful in showing how to allocate a
// buffer when sending messages between client and server.
//

#include <cstring>
#include <iostream>
#include <memory>
using namespace std;

int main() {
   const char* const data = "$RCSfile: makeunique.cpp,v $";
   const size_t datalen = strlen (data) + 1;
   auto buffer = make_unique<char[]> (strlen (data) + 1);
   strncpy (buffer.get(), data, datalen);
   cout << "buffer=\"" << buffer.get() << "\"" << endl;
}

//TEST// alias grind='valgrind --leak-check=full --show-reachable=yes'
//TEST// grind makeunique >makeunique.out 2>&1
//TEST// mkpspdf makeunique.ps makeunique.cpp* makeunique.out

