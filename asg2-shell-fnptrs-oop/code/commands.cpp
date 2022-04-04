// $Id: commands.cpp,v 1.20 2021-01-11 15:52:17-08 - - $

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words) {
   //cout << "dd";
   
   if(words.size() <= 1 ){
      throw command_error(" no pathname is specified");
      //cout << "ic";
       return;
   } else if(state.getcwd()->getcontents()->get_data()){
     throw command_error(" is a plain file"); 
     return; 
   }
   
   int size = words.size();
   map<string,inode_ptr> *m;
   m = state.getcwd()->getcontents()->get_dirents();
   for(int i = 1; i < size; ++i){
      string str = words[i];
       int flag = 0; 
   
   for(auto &j : *m){
      
      if (j.first != words[i] ){
         continue;
      }
         // use last vector of a path as a key for map 
      else if(j.second->getfiletype() == file_type::DIRECTORY_TYPE){
         flag = 2; 
         break; 
       }
       else{
       flag  = 1; 
       wordvec da = j.second->getcontents()->readfile();
       int da_size = da.size();
       for (int is = 0 ; is < da_size ; ++is ){
            cout << da[is] << " ";
       }
       cout << endl ; 
       
         }
   }
   //cat: food: No such file or directory
   string message = "cat: ";
    message += words[i];
   //string exit = "0";
   if(flag == 0){
       message += ": No such file or directory";
       // exit = "1";
      cout << message << endl;
      //exit_status_message(); 
   }
   else if(flag == 2){
      message += ": a directory";
      //exit = "1"; 
      cout << message << endl; 
      //exit_status_message() ;
   }
         
   }
 
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd (inode_state& state, const wordvec& words) {
   inode_ptr c_ptr = state.getcwd();
   vector <string> stat = state.track;
    inode_ptr curr = state.getcwd();
   
   // If no pathname is
   // specified, the root directory (/) is used
   if(words.size() <= 1 ){
      
       state.setcwd(state.getroot()); 
       state.track = {};
       return; 
   } 

   // if the pathname does not exist
   wordvec wor = split (words[1], "/");    

   int size = wor.size(); 
   
   //cout << to_string(size) << endl;
   if (wor.size() == 0){
     state.setcwd(state.getroot()); 
       state.track = {};
       return;  
   }
   map<string,inode_ptr> *curr_map;
   curr_map = curr->getcontents()->get_dirents();  
   int id = 0;                //0 1 
   //int farthest_p = size - 1; //0
   string p_str= wor[0];
  
   if (p_str == "."){
     auto sp = curr->getcontents()->get_dot()->lock(); 
        if (sp ){
           state.setcwd(sp);
        }
        if(size == 1){
          
            return;  
        }
       id  = 1; 
      }
      if(p_str == ".."){
    
      auto sp = curr->getcontents()->get_dotdot()->lock();
       if (sp ){
           //cout << "this pass" << endl;
           state.setcwd(sp);
           state.track.pop_back();
            
        } 
        else{
         if (state.track.size()>0){
         state.track.pop_back();
         }
        
         state.setcwd(state.getroot());
        }
        if(size == 1){
            // cout << "size is one" << endl;
            return;  
        }
         id =1;
         
      }
  // p_str = wor[0];
  //cout << "e" << endl; 
  
   while( curr-> getfiletype() == file_type::DIRECTORY_TYPE){
   
      int flag = 0; 
      for(auto &j : *curr_map ){
          //cout << j.first << wor[id] << endl; 
          //cout << j.first << endl;
         if(j.first == wor[id]){
           // cout << "foudn" << endl ;
            id++;
            flag = 1; 
            p_str = j.first; 
             curr_map = curr ->getcontents()->get_dirents();
            map<string,inode_ptr> c = *curr_map; 
            curr = c[p_str];
            curr_map = curr ->getcontents()->get_dirents();
             
         }
      }
      if(flag == 0){
         break;
      }
      
      }
  
   int error_flag = 0; 
    //cout << to_string(id) << endl;
   if (id != size ){
     //cout << to_string(id) << endl;
      error_flag = 1; 
   }
   // if it's the plain file 
    if (curr->getcontents()->get_data() ){
      error_flag = 1; 
   }
   
   if (error_flag == 1){
      throw command_error("error");
      return; 
   }
   // curr now the pointer for foo
   //cout << state.getcwd() << " " << curr << endl;
      state.setcwd(curr);
      //cout << "this" << p_str << endl
      //cout << to_string(size) << endl; 
      //int temp = words[1].size();

      for(int i = 0 ; i < size ; i++){
         //cout << "this is " << wor[i] ;
          state.track.push_back(wor[i]);
      }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words) {
   // no need to change
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words) {
   if (words.size() <= 1){
     exec::status(0);
     throw ysh_exit();
     return;  
   }
   int flag = 0; 
   int si = words.size();
   for(int i = 1; i < si; i++){
      int siz = words[i].length();
      for(int j = 0; j< siz; j++) {
      if (!isdigit(words[i].at(j))){
         flag = 1; 
         
      }
       }
   }
   if(flag == 1){
     exec::status(127); 
   }
   else{
      int in = stoi(words[1]);
      exec::status(in);
   }

   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}



// output consists of the inode number, then the size,
//then the filename
void fn_ls (inode_state& state, const wordvec& words) {
   // compare the words with path 
   wordvec wor;
   if(words.size() > 1 ){
   wor = split (words[1], "/");   
   }   
   inode_ptr oldcwd = state.getcwd();
   vector <string> oldtrack = state.track; 
 
   if(wor.size()>= 1){
   fn_cd(state, words);
   }
    
   int size = state.track.size();
   if (wor.size()!= 0 && size != 0
    && wor[0] != ".."  &&  wor[0] != "."){
   if (state.track[size -1] != wor[wor.size()-1]){
       state.setcwd(oldcwd);
       state.track = oldtrack;
       throw command_error("invalid path "); 
      return; 
   }
   }else if(wor.size()!= 0 && size == 0  
   && wor[0] != ".." &&  wor.size() >= 1 &&  wor[0] != "."){
     state.setcwd(oldcwd);
       state.track = oldtrack; 
       throw command_error("invalid path ");
      return;  
   }
   
   string pat = "";
   
   if(state.track.size() == 0){
      cout << "/:" << endl; 
   }else{
   
   int t_size = state.track.size();
   for(int i = 0 ; i <t_size ; i++){
      pat += "/";
      pat += state.track[i];
   }
   pat += ":";
   cout << pat << endl; 
   }

   inode_ptr dots = state.getcwd()->getcontents()->get_dot()->lock();
   if (dots == nullptr){
      dots = state.getcwd();
   }
   inode_ptr dotdots;
   dotdots = state.getcwd()->getcontents()->get_dotdot()->lock(); 
   cout << to_string(dots->get_inode_nr()) 
      << "       " 
      << to_string(dots->getcontents()->size()) 
      << "  " 
      << "./" << endl;
   if (dotdots == nullptr){
      dotdots = dots; 
   }
    cout << to_string(dotdots->get_inode_nr()) 
      << "       " 
      << to_string(dotdots->getcontents()->size()) 
      << "  " 
      << "../" << endl;

   map<string,inode_ptr> *curr_map;
   curr_map = state.getcwd()->getcontents()->get_dirents(); 
  
   for(auto &it : *curr_map){

      if(it.second->getfiletype() == file_type::DIRECTORY_TYPE){
      cout << to_string(it.second-> get_inode_nr())
       << "       "  
      << to_string(it.second->getcontents()->size())
       << "  "  
      << it.first << "/" << endl;
      }
      else{
         cout << to_string(it.second-> get_inode_nr())
       << "       "  
      << to_string(it.second->getcontents()->size())
       << "  "  
      << it.first << endl;

      }
   }

   state.setcwd(oldcwd);
   state.track = oldtrack; 
 //  cout << state.getcwd()->
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void helper (shared_ptr<inode> pointer, wordvec & track){
      string pat = "";
   
   if(track.size() == 0){
      cout << "/:" << endl; 
   }else{
   
   int t_size = track.size();
   for(int i = 0 ; i <t_size ; i++){
      pat += "/";
      pat += track[i];
   }
   pat += ":";
   cout << pat << endl; 
   }

   inode_ptr dots = pointer->getcontents()->get_dot()->lock();
   if (dots == nullptr){
      dots = pointer;
      
   }
   inode_ptr dotdots;
   dotdots = pointer->getcontents()->get_dotdot()->lock(); 
   cout << to_string(dots->get_inode_nr()) 
      << "       " 
      << to_string(dots->getcontents()->size()) 
      << "  " 
      << "./" << endl;
   if (dotdots == nullptr){
      dotdots = dots; 
      
   }
    cout << to_string(dotdots->get_inode_nr()) 
      << "       " 
      << to_string(dotdots->getcontents()->size()) 
      << "  " 
      << "../" << endl;


   map<string,inode_ptr> *curr_map;
   curr_map = pointer->getcontents()->get_dirents(); 
  
   for(auto &it : *curr_map){
   
      if(it.second->getfiletype() != file_type::DIRECTORY_TYPE){
       cout << to_string(it.second-> get_inode_nr())
       << "       "  
      << to_string(it.second->getcontents()->size())
       << "  "  
      << it.first << endl;
      
      }
      else{
         cout << to_string(it.second-> get_inode_nr())
       << "       "  
      << to_string(it.second->getcontents()->size())
       << "  "  
      << it.first << "/" << endl;
 
      //if(it.second->getfiletype() == file_type::DIRECTORY_TYPE){
     
      }
      
   }
//  for(auto &it : *curr_map){
//        if(it.second->getfiletype() == file_type::DIRECTORY_TYPE){
//          string t = "";
//          int size = track.size() ;
//        for(int i = 0; i < size ; i++){
//           t += track[i];
//             t += "\t";
//          }
//          t += it.first; 
//          wordvec pass = split(t, "\t");
//          //cout << t << endl; 
//          helper(it.second, pass);
//       }
//     }

  
   
}
void fn_lsr (inode_state& state, const wordvec& words) {
   
   // getcwd would refer to the pointer of o. then, nothing
   //map<string,inode_p
    wordvec wor;
   if(words.size() > 1 ){
   wor = split (words[1], "/");   
   }   
   inode_ptr oldcwd = state.getcwd();
   vector <string> oldtrack = state.track; 
 
   if(wor.size()>= 1){
   fn_cd(state, words);
   }
    
   int size = state.track.size();
   if (wor.size()!= 0 && size != 0 
   && wor[0] != ".."  &&  wor[0] != ".."){
   if (state.track[size -1] != wor[wor.size()-1]){
       state.setcwd(oldcwd);
       state.track = oldtrack;
       throw command_error("invalid path "); 
      return; 
   }
   }else if(wor.size()!= 0 && size == 0  
   && wor[0] != ".." &&  wor.size() >= 1 &&  wor[0] != ".."){
     state.setcwd(oldcwd);
       state.track = oldtrack; 
       throw command_error("invalid path ");
    return; 
   }

  helper (state.getcwd(), state.track);

  map<string,inode_ptr> *curr_map;

   //int depth = 1; 
   //int flag = 1; 
   //state.getcwd()->getfiletype() == file_type::DIRECTORY_TYPE 
   string t = "";
  wordvec pass; 
   inode_ptr o  = state.getcwd(); 
//   int flag = 1; 

//     while(flag == 1){
        curr_map =state.getcwd()->getcontents()->get_dirents(); 
//        int flag_2 = 0; 
for(auto &it : *curr_map){
        if(it.second->getfiletype() == file_type::DIRECTORY_TYPE){
         t= "";
             o = it.second;
             int track_size = state.track.size() ;
              for(int i = 0; i < track_size ; i++){
                 t += state.track[i];
                 //cout << state.track[i] << endl; ;
                 t += "\t";
                }
             t += it.first; 
              pass = split(t, "\t");
              //cout <<"this is pwd" << t << endl ;
             helper(o, pass); 
        }
}
  //           map<string,inode_ptr>  *pow;
  //         pow = o->getcontents()->get_dirents(); 
//              for(auto &iio : *pow){ 
//              
//                state.setcwd(it.second);
//                t= "";
//                track_size = state.track.size() ;
//              for(int i = 0; i < track_size ; i++){
//                 t += state.track[i];
//                 //cout << state.track[i] << endl; ;
//                   t += "\t";
//                }
//                 t += it.first; 
//                pass = split(t, "\t"); 
//                cout << t << endl; 
//                state.track = pass;
//                flag_2 = 1;
//                break;
//              }else{
//                 flag_2 = 0; 
//              }
//       }
     
//    } if(flag_2 == 1){
//          continue;
//       }else{
//    flag = 0;
//       } 
//    }
   state.setcwd(oldcwd);
   state.track = oldtrack;    
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words) {

   wordvec wor;
   if(words.size() > 1 ){
   wor = split (words[1], "/");   
   }else{
      command_error( "nothing to make" );
      return; 
   }  

   inode_ptr oldcwd = state.getcwd();
   vector <string> oldtrack = state.track; 
   string copy_words = "";
   // cd for 0 to wor.size() - 1;
   int W_size = wor.size();
   wordvec w_data;
   if(W_size>= 1){
      copy_words += "fn_cd\t";
      for(int i = 0 ; i < W_size - 1 ; ++i ){
          copy_words += wor[i];
          copy_words+= "/"; 
      }
      w_data = split(copy_words, "\t"); 
      //cout << copy_words << endl;
      if(w_data.size()>1){
      fn_cd(state, w_data);
      }
      
   }else{
      command_error( "no file to make" ); 
      return;
   }
   
   
   if(w_data.size() > 1 ){
   copy_words = w_data[1];
   w_data = split(copy_words, "/");  
   }
   int T_size = state.track.size();
   int w_size  = w_data.size();

   if (w_data.size() <= 1 ){
      w_size = -1;
   } 
   if ( w_size > 0){

   if ( w_data[0] != ".." ){
     int temp = oldtrack.size() ;
   if (T_size - temp != w_size ){
         //path doesn't exist 
         state.setcwd(oldcwd);
         state.track = oldtrack;  
         throw command_error("invalid path ");
         return; 

         }
   }
   }
  
   // if the one before path is plain file
   if (state.getcwd()->getfiletype() == file_type::PLAIN_TYPE){
      state.setcwd(oldcwd);
      state.track = oldtrack; 
     throw command_error("invalid path ");
      return;  
   }

  
   string last = wor[wor.size() -1];

   map<string,inode_ptr> *curr_map;
   map<string,inode_ptr> c ; 
   //cout << "in" << endl ; 
   curr_map = state.getcwd()->getcontents()->get_dirents();  
   c = *curr_map; 
   /// what to write in file 

   string in = "";
   if(words.size() > 2 ){
      int ss = words.size();
      for (int i = 2; i < ss ; ++i ){
         in += words[i];
         in += "\t";
      }
   }
    w_data = split(in, "\t");
    //cout << in << endl; 
   //////////

   for (auto &t : c){
      if(t.first == last ){
         if ( t.second->getfiletype() == file_type::PLAIN_TYPE){
         // if the path exist but the file already exist
         state.getcwd()->getcontents()->writefile(w_data);
         state.setcwd(oldcwd);
         state.track = oldtrack; 
         throw file_error("file already exist ");
         return;  
         }
         else{
           state.setcwd(oldcwd);
          state.track = oldtrack; 
         throw file_error("directory with same name exist ");
         return;   
         }
         
      }
   }

   inode_ptr curr_ptr;
  
   curr_ptr = state.getcwd()->getcontents()->mkfile(last);
   
   curr_ptr->getcontents()->writefile(w_data);
     //cout << "this is cd" << endl; 
   state.setcwd(oldcwd);
  
   state.track = oldtrack;   
   return ;  
    DEBUGF ('c', state);
    DEBUGF ('c', words);
   
}

void fn_mkdir (inode_state& state, const wordvec& words) {
   // cwd->
   wordvec wor;
   if(words.size() > 1 ){
   wor = split (words[1], "/");   
   }   

   inode_ptr oldcwd = state.getcwd();
   vector <string> oldtrack = state.track; 
    string copy_words = "";
   // cd for 0 to wor.size() - 1;
   int W_size = wor.size();
   wordvec w_data;
   if(W_size>= 1){
      copy_words += "fn_cd\t";
      for(int i = 0 ; i < W_size - 1 ; ++i ){
          copy_words += wor[i];
         // cout << "this are the word" << wor[i] <<  endl; 
          copy_words+= "/"; 
      }
      
      w_data = split(copy_words, "\t"); 
    
      if(w_data.size()> 1){ 
      fn_cd(state, w_data);
      }
      //cout << state.getcwd() << endl; 
   }else{
      throw command_error("no directory to make" ); 
      return;
   }
   if(w_data.size() > 1 ){
   copy_words = w_data[1];
   w_data = split(copy_words, "/");  
   }
   int T_size = state.track.size();
   int w_size  = w_data.size();
   if (w_data.size() <= 1 ){
      w_size = -1;
   }
    // new path = foo/sk 2
    // old one = {} 0
    // w_data = {foo, sk } 2

    // new path = {} 0
    // old path = {} 0 
    // w_data = {} 0

    // new_path = {foo} 1, old = {} 0, w_data = {foo} 1
    
    
   if ( w_size > 0){

   if ( w_data[0] != ".." ){
     int temp = oldtrack.size();
   if (T_size - temp != w_size ){
         //path doesn't exist 
         state.setcwd(oldcwd);
         state.track = oldtrack;  
         throw command_error("invalid path ");
         return; 

         }
   }
   }
   // if the one before path is plain file
   if (state.getcwd()->getfiletype() == file_type::PLAIN_TYPE){
      state.setcwd(oldcwd);
      state.track = oldtrack; 
     throw command_error("invalid path ");
      return;  
   }

  
   string last = wor[wor.size() -1];

   map<string,inode_ptr> *curr_map;
   map<string,inode_ptr> c ; 
   //cout << "in" << endl ; 
   curr_map = state.getcwd()->getcontents()->get_dirents();  
   c = *curr_map;  

   //cout << last << endl; 
   inode_ptr curr_ptr;
   curr_ptr = state.getcwd()->getcontents()->mkdir(last); 

   weak_ptr <inode> *doo = curr_ptr->getcontents()->get_dotdot();
    //curr_map = curr->getcontents()->get_dirents();
    //c = *curr_map;
    *doo = state.getcwd(); 
     c.clear(); 
   state.setcwd(oldcwd);
   state.track = oldtrack; 
   return;
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words) {
   int size = words.size();
   string prompt = "";
   for( int i = 1; i < size ; ++i ){
      prompt += words[i] ;
      prompt +=  " ";
   }
 
   //prompt ;
   state.setprompt(prompt);

   //cout << state.getprompt() << endl;
   // = prompt(str);
   if (words.size() == 1){
      state.setprompt("% ");
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words) {
   if(state.track.size() == 0){
      cout << "/" << endl; 
      return; 
   }
   cout << "/" << state.track[state.track.size() - 1] << endl ; 
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words) {
   
    wordvec wor;
   if(words.size() > 1 ){
   wor = split (words[1], "/");   
   }   
   inode_ptr oldcwd = state.getcwd();
   vector <string> oldtrack = state.track; 
 
   if(wor.size()>= 1){
   fn_cd(state, words);
   }
    
   int size = state.track.size();
   if (wor.size()!= 0 && size != 0
    && wor[0] != ".."  &&  wor[0] != "."){
   if (state.track[size -1] != wor[wor.size()-1]){
       state.setcwd(oldcwd);
       state.track = oldtrack; 
       throw command_error("invalid path ");
      return;  
   }
   }else if(wor.size()!= 0 && size == 0  
   && wor[0] != ".." &&  wor.size() >= 1 &&  wor[0] != "."){
     state.setcwd(oldcwd);
       state.track = oldtrack; 
       throw command_error("invalid path ");
      return;  
   }
   map<string,inode_ptr> *curr_map;
   curr_map = state.getcwd()->getcontents()->get_dirents(); 
  
   for(auto &it : *curr_map){
      if (it.second != nullptr){
   state.setcwd(oldcwd);
   state.track = oldtrack; 
   throw command_error("directory is not empty");
      return;  
      }

   }
   inode_ptr r = state.getcwd()->getcontents()->get_dotdot()->lock();
   r->getcontents()->get_dirents()->clear();
   state.setcwd(oldcwd);
   state.track = oldtrack; 
   
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

