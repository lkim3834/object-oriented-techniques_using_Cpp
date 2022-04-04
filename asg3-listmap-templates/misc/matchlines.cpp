// $Id: matchlines.cpp,v 1.3 2021-05-05 20:39:00-07 - - $

//
// Matches lines according to the program specs.
// Prints out results depending on type of line.
//

#include <cassert>
#include <iostream>
#include <regex>
#include <string>
using namespace std;

int main() {
   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
   for (;;) {
      string line;
      getline (cin, line);
      if (cin.eof()) break;
      cout << "input: \"" << line << "\"" << endl;
      smatch result;
      if (regex_search (line, result, comment_regex)) {
         cout << "Comment or empty line." << endl;
      }else if (regex_search (line, result, key_value_regex)) {
         cout << "key  : \"" << result[1] << "\"" << endl;
         cout << "value: \"" << result[2] << "\"" << endl;
      }else if (regex_search (line, result, trimmed_regex)) {
         cout << "query: \"" << result[1] << "\"" << endl;
      }else {
         assert (false and "This can not happen.");
      }
   }
   return 0;
}

/*
//TEST// (echo '    # comment   ';
//TEST//  echo '                  ';
//TEST//  echo '    No equal in this line.           ';
//TEST//  echo '   key key key  =  value value value  ';
//TEST//  echo 'only key here =     ';
//TEST//  echo '  = only value here     ';
//TEST//  echo '   =   '
//TEST// ) | matchlines >matchlines.out 2>&1
//TEST// mkpspdf matchlines.ps matchlines.cpp* matchlines.out
*/

