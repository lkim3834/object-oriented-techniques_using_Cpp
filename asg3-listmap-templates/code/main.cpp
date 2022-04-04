// $Id: main.cpp,v 1.13 2021-02-01 18:58:18-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <regex>
#include <cassert>
#include <fstream> 
#include <vector>
using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"
 
using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

const string cin_name = "-";

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

// string getinput (istream& cin){
//   for(;;) {
//       string line; 
//    getline (cin, line);
//     if (cin.eof()) break;
//   }
//   return line; 
// }
int main (int argc, char** argv) {
   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
    str_str_map test;
   //int argv_int = 1;
   //string line; 

vector<string> filenames (&argv[1], &argv[argc]);
if(filenames.size() == 0 ){
 string line;
 int count = 0; 
for (;;) {   
      count ++;   
      getline (cin, line);
      if (cin.eof()) break;
      cout << "-: " << to_string(count) << ": " << line << endl; 
      smatch result;
      if (regex_search (line, result, comment_regex)) {
         // #
      }else if (regex_search (line, result, key_value_regex)) {
         // key= value , = , = value
         // key = 
         if(result[2] == "" && result[1] != ""){
            // key and value deleted from map
            // test.key_type  = result[1];
            for(auto beg = test.begin(); beg != test.end(); ++ beg){
                 if(beg->first == result[1]){
                          test.erase(beg);
                 }
            }
         } 
         // = value 
         else if(result[1] == "" && result[2] != ""){
            // All of the key and value pairs with the 
             for(auto beg = test.begin(); beg != test.end(); ++ beg){
                 if(beg->second == result[2]){
                     cout << beg->first <<" = " << beg->second << endl; 
                 }
             }
         }
         // = 
         else if (result[1] == "" && result[2] == ""){
           for(auto beg = test.begin(); beg != test.end(); ++ beg){
                    cout << beg->first << " = " <<beg->second << endl;
           }
         }
         else{
             // key = value 
             test.insert( { result[1],result[2] });
             auto i = test.begin();
             ++ i ;
             cout <<result[1]  << " = "<<result[2]  << endl;
             }
      }else if (regex_search (line, result, trimmed_regex)) {
         // key 
        // if (result[2]  == "" && result[3] == ""){
               if (auto i= test.find(result[1])){
                   cout << i->first << " = " << i->second << endl;  
               }else{
               cout << result[1] <<": key not found" << endl; 
               }
      }
      else {
         assert (false and "This can not happen.");
      }
       
   }
}else{
for (const auto& filename: filenames) {
 string line;
 ifstream myfile;
 int count = 0; 
if(filename != cin_name || filenames.size() > 0) {
  myfile.open(filename);
}
for (;;) {
       
   if(filename == cin_name ) {
      //cout << filename << endl; 
      getline (cin, line);
      if (cin.eof()) break;
      cout << "-: 1: " << line << endl; 
   }
   else{
       
       if ( myfile.fail()){
         
        complain() <<  filename <<": No such file or directory" << endl;
         break;
        }else{
           getline (myfile, line); 
           count ++; 
          
           if(myfile.eof()){
              break;
              myfile.close();
           }
            cout << filename << ": " << to_string(count) << ": " << line<< endl; 
        }
     }
  

      smatch result;
      if (regex_search (line, result, comment_regex)) {
         // #
      }else if (regex_search (line, result, key_value_regex)) {
         // key= value , = , = value
         // key = 
         if(result[2] == "" && result[1] != ""){
            // key and value deleted from map
            // test.key_type  = result[1];
            for(auto beg = test.begin(); beg != test.end(); ++ beg){
                 if(beg->first == result[1]){
                          test.erase(beg);
                 }
            }
         } 
         // = value 
         else if(result[1] == "" && result[2] != ""){
            // All of the key and value pairs with the 
             for(auto beg = test.begin(); beg != test.end(); ++ beg){
                 if(beg->second == result[2]){
                     cout << beg->first <<" = " << beg->second << endl; 
                 }
             }
         }
         // = 
         else if (result[1] == "" && result[2] == ""){
           for(auto beg = test.begin(); beg != test.end(); ++ beg){
                    cout << beg->first << " = " <<beg->second << endl;
           }
         }
         else{
             // key = value 
             test.insert( { result[1],result[2] });
             auto i = test.begin();
             ++ i ;
             cout <<result[1]  << " = "<<result[2]  << endl;
             }
      }else if (regex_search (line, result, trimmed_regex)) {
         // key 
        // if (result[2]  == "" && result[3] == ""){
               if (auto i= test.find(result[1])){
                   cout << i->first << " = " << i->second << endl;  
               }else{
               cout << result[1] <<": key not found" << endl; 
               }
      }
      else {
         assert (false and "This can not happen.");
      }
       
   }
}
}
   return EXIT_SUCCESS;
}

