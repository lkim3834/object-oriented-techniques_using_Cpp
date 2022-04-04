// $Id: file_sys.h,v 1.8 2020-10-22 14:37:26-07 - - $

#ifndef __INODE_H__
#define __INODE_H__

#include <exception>
#include <iostream>
#include <memory>
#include <map>
#include <vector>

using namespace std;

#include "util.h"

// inode_t -
//    An inode is either a directory or a plain file.

enum class file_type {PLAIN_TYPE, DIRECTORY_TYPE};
class inode;
class base_file;
class plain_file;
class directory;
using inode_ptr = shared_ptr<inode>;
using base_file_ptr = shared_ptr<base_file>;
ostream& operator<< (ostream&, file_type);


// inode_state -
//    A small convenient class to maintain the state of the simulated
//    process:  the root (/), the current directory (.), and the
//    prompt.

// shared_ptr from library works like a pointer: maintains reference count

// p = make_shared....
// -> create a object and set the reference count = 1;
// q = p 
// reference count = 2
// if q goes out of scope, reference -- and p gets deleted


class inode_state {
   friend class inode;
   friend ostream& operator<< (ostream& out, const inode_state&);
   private:
      inode_ptr root {nullptr};
      inode_ptr cwd {nullptr};
      string prompt_ {"% "};
      
   public:
      // adding vector to keep track of what directory you are in
      // used for pwd and cd
      //setter
      void setprompt(string p){
         prompt_ = p;
      }
      //getter
      string getprompt(){
         return prompt_;
      }
      //pwd
      vector <string> track;
      // const wordvec& track = {};

      //setter for root
      void setroot(inode_ptr p ){
         root = p;
      }
      //getter for root
      inode_ptr getroot(){
         return root;
      }
      //setter for cwd
      void setcwd(inode_ptr p ){
         cwd = p;
      }
      //getter for cwd
      inode_ptr getcwd(){
         return cwd;
      }
      inode_state (const inode_state&) = delete; // copy ctor
      inode_state& operator= (const inode_state&) = delete; // op=
      // constructor 
      inode_state();
      // returns the prompt
      const string& prompt() const;
      // setter
      void prompt (const string&);
};

// class inode -
// inode ctor -
//    Create a new inode of the given type.
// get_inode_nr -
//    Retrieves the serial number of the inode.  Inode numbers are
//    allocated in sequence by small integer.
// size -
//    Returns the size of an inode.  For a directory, this is the
//    number of dirents.  For a text file, the number of characters
//    when printed (the sum of the lengths of each word, plus the
//    number of words.
//    

class inode {
   friend class inode_state;
   private:
      static size_t next_inode_nr;
      size_t inode_nr;
      base_file_ptr contents;
      file_type filetype; 

   public:
      void setcontents(base_file_ptr p){
         contents = p;
      }
      //getter
      base_file_ptr getcontents(){
         return contents;
      }    

      void setfiletype( file_type p){
         filetype = p;
      }
      //getter
      file_type getfiletype(){
         return filetype;
      }    
       
      inode (file_type);
      size_t get_inode_nr() const;
};


// class base_file -
// Just a base class at which an inode can point.  No data or
// functions.  Makes the synthesized members useable only from
// the derived classes.

class file_error: public runtime_error {
   public:
      explicit file_error (const string& what);
};

class base_file {
   protected:
     base_file() = default;
      virtual const string& error_file_type() const = 0;
   public:
      wordvec *d; 
      map<string,inode_ptr> *m; 
     
      //  weak_ptr<inode>dot_ ;
      //   weak_ptr<inode>dotdot_ ; 
      virtual wordvec* get_data() const{return d;};
      virtual map<string,inode_ptr> * get_dirents() const{return m;} ;
      virtual weak_ptr <inode>* get_dotdot() const {return nullptr; }
     virtual weak_ptr <inode> *get_dot() const {return nullptr;}  
       
      virtual ~base_file() = default;
      base_file (const base_file&) = delete;
      base_file& operator= (const base_file&) = delete;
      virtual size_t size() const = 0;
      virtual const wordvec& readfile() const;
      virtual void writefile (const wordvec& newdata);
      virtual void remove (const string& filename);
      virtual inode_ptr mkdir (const string& dirname);
      virtual inode_ptr mkfile (const string& filename);
};

// class plain_file -
// Used to hold data.
// synthesized default ctor -
//    Default vector<string> is a an empty vector.
// readfile -
//    Returns a copy of the contents of the wordvec in the file.
// writefile -
//    Replaces the contents of a file with new contents.

class plain_file: public base_file {
   private:
      wordvec data;
      
      virtual const string& error_file_type() const override {
         static const string result = "plain file";
         return result;
      }
   public:
     wordvec *da = &data;
     virtual const wordvec* get_data() { return da;}
      virtual size_t size() const override;
      virtual const wordvec& readfile() const override;
      virtual void writefile (const wordvec& newdata) override;
};

// class directory -
// Used to map filenames onto inode pointers.
// default ctor -
//    Creates a new map with keys "." and "..".
// remove -
//    Removes the file or subdirectory from the current inode.
//    Throws an file_error if this is not a directory, the file
//    does not exist, or the subdirectory is not empty.
//    Here empty means the only entries are dot (.) and dotdot (..).
// mkdir -
//    Creates a new directory under the current directory and 
//    immediately adds the directories dot (.) and dotdot (..) to it.
//    Note that the parent (..) of / is / itself.  It is an error
//    if the entry already exists.
// mkfile -
//    Create a new empty text file with the given name.  Error if
//    a dirent with that name exists.

class directory: public base_file {
   private:
      // Must be a map, not unordered_map, so printing is lexicographic
      //map<string, shared_ptr<inode>> shared_dirents; // all subfolders and files
      //map<string, weak_ptr<inode>> weak_dirents;      // all subfolder, files, dot, and dotdot ex ba2, f1, f2, . , .. , 
      
      virtual const string& error_file_type() const override {
         static const string result = "directory";
         return result;
      }
      //virtual get_dirents() const override; 
   public:
      map<string,inode_ptr> dirents;
      
        weak_ptr<inode> dot_; 
       weak_ptr<inode> dotdot_ ; 
       weak_ptr<inode>*dot =  &dot_ ;
       weak_ptr<inode>*dotdot =  &dotdot_ ;
      
      map<string,inode_ptr> *f= &dirents;
      map<string,inode_ptr> *get_dirents() const override {return f;};
      weak_ptr <inode> *get_dotdot() const override{ 
        
         return dotdot;
         }
      weak_ptr<inode> * get_dot() const override{
          
          return dot;
          }  
  

      virtual size_t size() const override;
      virtual void remove (const string& filename) override;
      virtual inode_ptr mkdir (const string& dirname) override;
      virtual inode_ptr mkfile (const string& filename) override;
};

#endif

