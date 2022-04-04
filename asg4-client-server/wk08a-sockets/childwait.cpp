// $Id: childwait.cpp,v 1.9 2021-05-12 21:46:12-07 - - $

//
// A zombie process is one that has exited but for which the
// parent process has not waited for it.  This process creates
// up to ten child processes and then waits for them.
//
// Usage: childwait children pdelay loops cdelays
// children - number of children to create
// pdelay   - parent delays (seconds) before creating each child
// loops    - number of times each child is to print a message
// cdelays  - delay (seconds) before each child's message
//

#include <cerrno>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>
using namespace std;

#include <libgen.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

string progname;
timeval start_time;

string when() {
   timeval now;
   gettimeofday (&now, NULL);
   double start = start_time.tv_sec + start_time.tv_usec / 1.0e6;
   double dnow = now.tv_sec + now.tv_usec / 1.0e6;
   double delta = dnow - start;
   stringstream diff;
   diff << fixed << setw(10) << setprecision(6) << delta
        << setw(6) << getpid();
   return diff.str();;
}


template <typename Type>
Type from_string (const string &str) { 
   stringstream stream; 
   stream << str; 
   Type result; 
   if (not (stream >> result and stream.eof())) {
      throw domain_error (string (typeid (Type).name())
            + " from_string (" + str + ")"); 
   }
   return result; 
}

void usage() {
   cerr << "Usage: " << progname << " children pdelay loops cdelays"
        << endl;
   exit (EXIT_FAILURE);
}

void error (const string& obj) {
   cerr << obj << ": " << string (strerror (errno)) << endl;
   exit (EXIT_FAILURE);
}

struct options {
   size_t children {};
   size_t pdelay {};
   size_t loops {};
   size_t cdelays {};
   options (const vector<string> opts) {
      if (opts.size() != 4) usage();
      try {
         children = from_string<size_t> (opts[0]);
         pdelay = from_string<size_t> (opts[1]);
         loops = from_string<size_t> (opts[2]);
         cdelays = from_string<size_t> (opts[3]);
      }catch (domain_error& error) {
         cerr << progname << ": " << error.what() << endl;
         usage();
      }
   }
};


pid_t wait_children() {
   for (;;) {
      int status;
      pid_t child_pid = waitpid (-1, &status, WNOHANG);
      if (child_pid <= 0) return child_pid;
      cout << when() << " Child pid " << child_pid
             << " status " << (status >> 8)
             << " signal " << (status & 0x7F)
             << " core " << (status >> 7 & 1) << endl;
   }
}

int run_child (size_t id, size_t loops, size_t cdelays) {
   cout << when() << " Child " << id << " starting." << endl;
   for (size_t loop = 0; loop < loops; ++loop) {
      sleep (cdelays);
      cout << when() << " Child " << id << " message " << loop
             << "." << endl;
   }
   cout << when() << " Child " << id << " finished." << endl;
   exit (id);
   // Must not return to main function.
}

int main (int argc, char** argv) {
   progname = basename (argv[0]);
   vector<string> args (&argv[1], &argv[argc]);
   gettimeofday (&start_time, NULL);
   options opts (args);
   cout << when() << " Main starting." << endl;
   for (size_t child = 0; child < opts.children; ++child) {
      pid_t pid = fork();
      if (pid < 0) error ("fork");
      if (pid == 0) run_child (child, opts.loops, opts.cdelays);
      cout << when() << " Child " << pid << " created." << endl;
      wait_children();
      sleep (opts.pdelay);
   }
   for (;;) {
      cout << when() << " Main waiting for child(ren)." << endl;
      pid_t child_pid = wait_children();
      if (child_pid < 0) break;
      sleep (opts.pdelay);
   }
   cout << when() << " Main finished." << endl;
}

//TEST// ./childwait 5 5 5 2 >childwait.out 2>&1
//TEST// mkpspdf childwait.ps childwait.cpp* childwait.out*

