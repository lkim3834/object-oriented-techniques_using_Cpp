// $Id: file_sys.cpp,v 1.10 2021-04-10 14:23:40-07 - - $

#include <cassert>
#include <utility>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cctype>
using namespace std;

#include "debug.h"
#include "file_sys.h"

size_t inode::next_inode_nr {1};

ostream& operator<< (ostream& out, file_type type) {
   switch (type) {
      case file_type::PLAIN_TYPE: out << "PLAIN_TYPE"; break;
      case file_type::DIRECTORY_TYPE: out << "DIRECTORY_TYPE"; break;
      default: assert (false);
   };
   return out;
}

inode_state::inode_state() {
   // inode_ptr t = make_shared<inode> (file_type)
   // is this the right way to initialize? How do I access the map 
    root = make_shared<inode> (file_type::DIRECTORY_TYPE);
    cwd = root;
    
    
   //prompt =  
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

const string& inode_state::prompt() const { return prompt_; }

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      
      case file_type::PLAIN_TYPE:
      
           contents = make_shared<plain_file>();
           filetype  = file_type::PLAIN_TYPE;
           break;

      case file_type::DIRECTORY_TYPE:
          
           contents = make_shared<directory>();
           filetype = file_type::DIRECTORY_TYPE; 
           break;

      default: assert (false);
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

size_t inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const {
   
   if(!get_dirents()->empty())   {
   throw file_error ("is a " + error_file_type());

   }
   
      return *get_data();
   
}

void base_file::writefile (const wordvec& filename) {
   
   if(!get_dirents()->empty() ){
      throw file_error ("is a " + error_file_type());
   }
   else{
      *get_data() = filename;
      
   }
   return;
}

//    Throws an file_error if this is not a directory, the file
//    does not exist, or the subdirectory is not empty.
//    Here empty means the only entries are dot (.) and dotdot (..).

void base_file::remove (const string& filename) {
   map<string,inode_ptr> *f = get_dirents();
   int flag = 0 ;
  inode_ptr s ; 
   for(auto &e : *f){
      if(e.first == filename){
         flag = 1; 
         s = e.second;
      }
   }
   if(flag == 0){
       throw file_error ("is a " + error_file_type());
       return;
   }

   
   flag = 0; 
  
   f = s->getcontents()->get_dirents();
   map<string,inode_ptr> fd =  *f;
   if (s->getcontents()->get_data()->size()!= 0 ){
      flag = 1; 
   }
   else{
    if(!fd.empty()){
       flag = 1;
    }
   }
   
   if (flag != 1){
      f->clear();
   }
   else{
      throw file_error ("is a " + error_file_type());
   }
  return ;
}

inode_ptr base_file::mkdir (const string& dirname) {

   // /foo/dje
   //.. points to the parent directory and 
   //. points to current directory
   //- initiialize inode_ptr
   //- add . and .. to its dirents
   //- then add the new directory to parent directory
    // inode pointer for inode of dirname
    //cout << dirname << endl; 
   wordvec *dad = get_data();
   
   return nullptr; 
   int size = dad->size(); 
   if (size!= 0){
        throw file_error ("is a " + error_file_type()); 
        return nullptr; 
       }
    directory dir; 
   // inode_ptr in = dir.mkdir(dirname);
    
    //cout << dirname; 
   return dir.mkdir(dirname);

}

inode_ptr base_file::mkfile (const string& filename) {
   //!mymap.empty()
  

   map<string,inode_ptr> *md = get_dirents();
   if (!md->empty()){
   throw file_error ("is a " + error_file_type());
   return nullptr;
   }
   plain_file plain;
   return plain.mkfile(filename);
}


size_t plain_file::size() const {
   size_t size {0};
   int d_size = data.size();
   int output = 0; 
   for(int i =0; i < d_size ; ++i){
      output += data[i].length();
      output ++;
   }
   if (output > 0){
   output -= 1;
   }
   size = output;
   // DEBUGF ('i', "size = " << size);
    return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   this->data = words; 
 
    return; 
}

size_t directory::size() const {
   size_t size {0};
   size = dirents.size();
   size += 2; 
   
   DEBUGF ('i', "size = " << size);
   return size;
}

//    Removes the file or subdirectory from the current inode.

void directory::remove (const string& filename) {
   // dirents is the map 
   // the map<string,inode_ptr> *f>
   inode_ptr s =  dirents[filename];
   map<string,inode_ptr> *fi = s->getcontents()->get_dirents();
   
   fi->clear();
   
   DEBUGF ('i', filename);
}


//    Creates a new directory under the current directory and 
//    immediately adds the directories dot (.) and dotdot (..) to it.
//    Note that the parent (..) of / is / itself.  It is an error
//    if the entry already exists.

inode_ptr directory::mkdir (const string& dirname) {
   ///dot is pointer of the inode i just made
   /// .. would be the ptr of the node you're calling mkdir on
  
   inode_ptr x = make_shared<inode>(file_type::DIRECTORY_TYPE);
   // map<string,inode_ptr> *fuc =  x->getcontents()->get_dirents();
   // map<string, inode_ptr> dui = *fuc; 
  // dirents["."] = x; 
   weak_ptr <inode> *doos = x->getcontents()->get_dot();
   dirents[dirname] = x; 
   *doos = x;
   //cout << dirname << endl; 
   //cout << dot_.lock() << endl; 

   //cout << "this di" << x<< endl; 
   // inode pointer would point to dotdot
   DEBUGF ('i', dirname);
   return x;
}


inode_ptr directory::mkfile (const string& filename) {
   // update dot and dotdot
   // dot would be the m[filename]
   inode_ptr y = make_shared<inode>( file_type :: PLAIN_TYPE);
   this->dirents[filename] = y; 
  
   DEBUGF ('i', filename);
   return y;
}

