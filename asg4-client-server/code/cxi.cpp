// $Id: cxi.cpp,v 1.5 2021-05-18 01:32:29-07 - - $

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream> 
#include <cstdlib>
#include <unistd.h>
using namespace std;
#include <iomanip>
#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include<inttypes.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<arpa/inet.h>


#include "debug.h"
#include "logstream.h"
#include "protocol.h"
#include "socket.h"

logstream outlog (cout);
struct cxi_exit: public exception {};
using wordvec = vector<string>;
unordered_map<string,cxi_command> command_map {
   {"exit", cxi_command::EXIT},
   {"help", cxi_command::HELP},
   {"ls"  , cxi_command::LS  },
   {"get" , cxi_command::GET},
{"rm"  , cxi_command::RM  },
   {"put" , cxi_command::PUT},


};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cxi_help() {
   cout << help;
}

void cxi_ls (client_socket& server) {
   cxi_header header;
   header.command = cxi_command::LS;
   DEBUGF ('h', "sending header " << header << endl);
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   DEBUGF ('h', "received header " << header << endl);
   //receive lsout/nak
   if (header.command != cxi_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      // read from header into local var`
      size_t host_nbytes = ntohl (header.nbytes);
      //set nbytes(based on header)
      //declare buffer
      auto buffer = make_unique<char[]> (host_nbytes + 1);
   //  size_t receiv = 0 ; 
//while (receiv != host_nbytes){
 
    recv_packet (server, buffer.get() , host_nbytes);
      DEBUGF ('h', "received " << host_nbytes << " bytes");
//    receiv += rec;     
 //} 
     buffer[host_nbytes] = '\0';
      cout << buffer.get();
   }
}

void cxi_get (client_socket& server , string & file) {
      cxi_header header;
      header.command = cxi_command::GET;
      DEBUGF ('h', "sending header " << header << endl);
     //auto si = file.length();
     char name[60];
      strcpy(name, file.c_str());
     //  count copies max of count char from src to dest 
      name[file.length()] = '\0';
      strncpy(header.filename, name, 59); 
     send_packet(server, &header, sizeof header);
     recv_packet (server, &header, sizeof header);
     DEBUGF ('h', "received header " << header << endl);
if (header.command != cxi_command::FILEOUT){
        outlog << "sent GET, server did not return FILEOUT" << endl;
     outlog << "server returned " << header << endl;
       }else{
       //check nbyte
      size_t host_nbytes = ntohl (header.nbytes);
      //set nbytes(based on header)
      //make buffer 
       auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      DEBUGF ('h', "received " << host_nbytes << " bytes");
      buffer[host_nbytes] = '\0';
      //wrtie file 
     ofstream outfile(header.filename);
       
   long size = host_nbytes; 
  outfile.write(buffer.get(), size );
      //header.filename = buffer;  
      outfile.close();
      outlog << "success"<< endl ;
       }

}
void cxi_put (client_socket& server , string & file) {

 cxi_header header;
//const char*  get_pipe = file ;
char name[60];
      strcpy(name, file.c_str());
     //  count copies max of count char from src to dest 
      name[file.length()] = '\0';
              strncpy(header.filename, name, 59);
       struct stat stat_buf;
       memset(&stat_buf, 0, sizeof stat_buf);
       int status = stat(name, &stat_buf);
       if(status!= 0 ){
          outlog << name << ": " << strerror (errno) << endl;
      //header.command = cxi_command::NAK;
      // header.nbytes = htonl (errno);
      //send_packet (server, &header, sizeof header);
      return;
      }
header.command = cxi_command::PUT;
//header.nbytes = htonl 
//header.nbytes = htonl (file.size());
       ifstream openfile (file,  ios::in|ios::binary|ios::ate);
   streampos size = openfile.tellg();
  // auto buffer = new char [stat_buf.st_size];
    //read in file 
   int si = size; 
auto buffer = make_unique<char[]> (si + 1);      
openfile.seekg (0, ios::beg);
      openfile.read(buffer.get(), size);
      buffer[size] = '\0';
   openfile.close();
string output = buffer.get();
header.nbytes = htonl (size);
send_packet(server, &header, sizeof header);
send_packet (server, output.c_str(), size);
for(int i = 0 ; i < size ; i ++){
 DEBUGF ('h', "received " <<  buffer[i] << " bytes");  
}
//size_t host_nbytes = ntohl (header.nbytes);
recv_packet(server, &header, sizeof header);
//recv_packet (server, buffer.get() , host_nbytes);

if (header.command != cxi_command::ACK){
      //        cout << header.command  << endl ;
   outlog << "sent PUT, server did not return FILEOUT" << endl;
   outlog << "server returned " << header << endl;
   }else{
outlog << "success" << endl ;
}



}

void cxi_rm (client_socket& server , string & file) {
cxi_header header;
 header.command = cxi_command::RM;
char name[60];
      strcpy(name, file.c_str());
     //  count copies max of count char from src to dest 
             name[file.length() ] = '\0';
                  strncpy(header.filename, name, 59);
send_packet(server, &header, sizeof header);
recv_packet(server, &header, sizeof header);
if (header.command != cxi_command::ACK){
              //cout << header.command  << endl ;
                 outlog << "sent RM, server did not return ACK" << endl;
                    outlog << "server returned " << header << endl;
                       }else{
                       outlog << "success" << endl ;
                   }
}
void usage() {
   cerr << "Usage: " << outlog.execname() << " host port" << endl;
   throw cxi_exit();
}

pair<string,in_port_t> scan_options (int argc, char** argv) {
   for (;;) {
      int opt = getopt (argc, argv, "@:");
      if (opt == EOF) break;
      switch (opt) {
         case '@': debugflags::setflags (optarg);
                   break;
      }
   }
   if (argc - optind != 2) usage();
   string host = argv[optind];
   in_port_t port = get_cxi_server_port (argv[optind + 1]);
   return {host, port};
}


//form asg2 util.cpp 

wordvec split (const string& line, const string& delimiters) {
   wordvec words;
   size_t end = 0;

   // Loop over the string, splitting out words, and for each word
   //    // thus found, append it to the output wordvec.
          for (;;) {
               size_t start = line.find_first_not_of (delimiters, end);
                     if (start == string::npos) break;
                     end = line.find_first_of (delimiters, start);
                    words.push_back (line.substr (start, end - start));
                                     }
                                          return words;
                                          }
  
int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << to_string (hostinfo()) << endl;
   try {
      auto host_port = scan_options (argc, argv);
      string host = host_port.first;
      in_port_t port = host_port.second;
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cxi_exit();
         outlog << "command " << line << endl;
         wordvec splited = split(line, " " );
//int si = splited.size()  ;      
// for(int i = 0 ; i < si; i++){ 
  //      cout << "line[0]" << splited[i] << endl;  
//}
        const auto& itor = command_map.find (splited[0]);
         cxi_command cmd = itor == command_map.end()
                         ? cxi_command::ERROR : itor->second;
         switch (cmd) {
            case cxi_command::EXIT:
               throw cxi_exit();
               break;
            case cxi_command::HELP:
               cxi_help();
               break;
            case cxi_command::LS:
               cxi_ls (server);
               break;
           case cxi_command::GET:
               cxi_get (server , splited[1]);
               break; 
           case cxi_command::PUT:
               cxi_put(server , splited[1]);
               break;
 case cxi_command::RM:
               cxi_rm (server , splited[1]);
               break;
            default:

               outlog << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      DEBUGF ('x', "caught cxi_exit");
   }
   return 0;
}

