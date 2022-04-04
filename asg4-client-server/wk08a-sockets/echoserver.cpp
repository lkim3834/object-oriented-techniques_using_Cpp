// $Id: echoserver.cpp,v 1.24 2021-05-13 00:56:40-07 - - $

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
using namespace std;

#include "socket.h"

string execname;

template <typename Type>
Type from_string (const string &str) { 
   stringstream stream; 
   stream << str; 
   Type result; 
   if (not (stream >> result and stream.eof())) {
      throw domain_error (string (typeid (Type).name()) + str + ")"); 
   }
   return result; 
}

void echo_server (in_port_t port) {
   server_socket listener (port);
   accepted_socket client;
   listener.accept (client);
   cout << execname << ": accepted connection: "
        << to_string (client) << endl;

   for (;;) {
      char buffer[1<<10]; 
      int nbytes = client.recv (buffer, sizeof buffer);
      if (nbytes == 0) break;
      client.send (buffer, nbytes);
      cout << execname << ": buffer \"";
      string sbuffer {buffer};
      for (char chr: sbuffer) {
         if (isprint (chr)) cout << chr;
                       else cout << "^" << char (chr ^ 0x40);
      }
      cout << "\"" << endl;
      if (sbuffer == "\x04") break;
   }
}

int main (int argc, char** argv) {
   execname = basename (argv[0]);
   if (argc != 2) {
      cerr << "Usage: " << execname << " port " << endl;
      exit (1);
   }
   cout << execname << " starting." << endl;
   echo_server (from_string<in_port_t> (argv[1]));
   cout << execname << " finished." << endl;
}

#include "socket.cpp"

