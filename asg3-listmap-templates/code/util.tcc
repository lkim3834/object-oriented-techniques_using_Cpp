// $Id: util.tcc,v 1.4 2020-02-06 12:33:29-08 - - $

#include <sstream>
#include <typeinfo>
using namespace std;

template <typename Type>
string to_string (const Type& that) {
   ostringstream stream;
   stream << that;
   return stream.str();
}

template <typename Type>
Type from_string (const string& that) {
   stringstream stream;
   stream << that;
   Type result;
   if (not (stream >> result and stream.eof())) {
      throw domain_error (string (typeid (Type).name())
            + " from_string (" + that + ")");
   }
   return result;
}

