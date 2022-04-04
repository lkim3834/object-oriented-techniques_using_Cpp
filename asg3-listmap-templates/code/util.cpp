// $Id: util.cpp,v 1.18 2020-02-06 12:55:59-08 - - $

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <stdexcept>
#include <string>
using namespace std;

#include "debug.h"
#include "util.h"

int sys_info::exit_status_ = EXIT_SUCCESS;
string sys_info::execname_; // Must be initialized from main().

void sys_info::execname (const string& argv0) {
   assert (execname_ == "");
   int slashpos = argv0.find_last_of ('/') + 1;
   execname_ = argv0.substr (slashpos);
   cout << boolalpha;
   cerr << boolalpha;
   DEBUGF ('u', "execname_ = " << execname_);
}

const string& sys_info::execname () {
   assert (execname_ != "");
   return execname_;
}

void sys_info::exit_status (int status) {
   assert (execname_ != "");
   exit_status_ = status;
}

int sys_info::exit_status () {
   assert (execname_ != "");
   return exit_status_;
}

ostream& complain() {
   sys_info::exit_status (EXIT_FAILURE);
   cerr << sys_info::execname () << ": ";
   return cerr;
}

void syscall_error (const string& object) {
   complain() << object << ": " << strerror (errno) << endl;
}

