// $Id: cxid.cpp,v 1.8 2021-05-18 01:32:29-07 - - $

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include "debug.h"
#include "logstream.h"
#include "protocol.h"
#include "socket.h"

logstream outlog (cout);
struct cxi_exit: public exception {};

void reply_ls (accepted_socket& client_sock, cxi_header& header) {
   static const char ls_cmd[] = "ls -l 2>&1";
   FILE* ls_pipe = popen (ls_cmd, "r");
 //cout << "reply ls " << endl;  
 if (ls_pipe == nullptr) { 
      outlog << ls_cmd << ": " << strerror (errno) << endl;
      header.command = cxi_command::NAK;
      header.nbytes = htonl (errno);
      send_packet (client_sock, &header, sizeof header);
      return;
   }
   string ls_output;
   char buffer[0x1000];
   for (;;) {
      char* rc = fgets (buffer, sizeof buffer, ls_pipe);
      if (rc == nullptr) break;
      ls_output.append (buffer);
   }
   pclose (ls_pipe);
   //send(lsout output)
   header.command = cxi_command::LSOUT;
   // convert local var into nbytes needed in header 
   // plan to send after the header 
   header.nbytes = htonl (ls_output.size());
   memset (header.filename, 0, FILENAME_SIZE);
   DEBUGF ('h', "sending header " << header);
   send_packet (client_sock, &header, sizeof header);
   send_packet (client_sock, ls_output.c_str(), ls_output.size());
   DEBUGF ('h', "sent " << ls_output.size() << " bytes");
}

void reply_get(accepted_socket& client_sock,cxi_header&  header){
       const char*  get_pipe = header.filename ;
       struct stat stat_buf; 
       memset(&stat_buf, 0, sizeof stat_buf); 
       int status = stat(get_pipe, &stat_buf);
       if(status!= 0 ){
          outlog << get_pipe << ": " << strerror (errno) << endl;
      header.command = cxi_command::NAK;
      header.nbytes = htonl (errno); 
     send_packet (client_sock, &header, sizeof header);
       return;
      }
 //    size_t host_nbytes = ntohl (header.nbytes);
  string file = header.filename;  
  ifstream openfile (file,  ios::in|ios::binary|ios::ate);
   streampos size = openfile.tellg(); 
   //auto buffer = new char [stat_buf.st_size];
    //read in file 
    char buffer[0x1000];
   openfile.seekg (0, ios::beg);
   openfile.read(buffer, size);

    DEBUGF ('h', "received " << stat_buf.st_size  << " bytes"); 
       //recv_packet ( client_sock, buffer.get(), host_nbytes);
     // DEBUGF ('h', "received " << host_nbytes << " bytes");
      buffer[stat_buf.st_size] = '\0';
       header.command = cxi_command::FILEOUT; 
          header.nbytes = htonl(stat_buf.st_size);
         //memset(header.filename, 0 , FILENAME_SIZE);
    openfile.close();
 DEBUGF ('h', "sending header " << header);
// send the file down the socket piecemeal 
    DEBUGF ('h', "sending buf " << 0 << ": " << buffer[0]);
     DEBUGF ('h', "sending size " << size << ": " << stat_buf.st_size);
string output = buffer;
   send_packet (client_sock, &header, sizeof header);
   send_packet (client_sock, output.c_str(), output.size());
   DEBUGF ('h', "sent " << size << " bytes");        

}
void reply_put(accepted_socket& client_sock,cxi_header&  header){

 //check nbytes
  size_t host_nbytes = ntohl (header.nbytes);
 //make buffer
auto buffer = make_unique<char[]> (host_nbytes + 1) ;  
//auto buffer = new char[];
  // receive nbytes data
  recv_packet(client_sock, buffer.get(), host_nbytes);
//  int ho = host_nbytes;

buffer[host_nbytes] = '\0';
ofstream outfile(header.filename);
//long size = host_nbytes; 
outfile.write( buffer.get() , host_nbytes);
outfile.close();
const char*  get_pipe = header.filename ;
       struct stat stat_buf;
       memset(&stat_buf, 0, sizeof stat_buf);
      
 int status = stat(get_pipe, &stat_buf);
DEBUGF ('h', "receive status" << status); 
      if(status!= 0 ){
          outlog << get_pipe << ": " << strerror (errno) << endl;
      header.command = cxi_command::NAK;
      header.nbytes = htonl (errno);
       
       send_packet (client_sock, &header, sizeof header);
//       send_packet(client_sock, &buffer , 0 );
      return;
      }
string output = buffer.get();
header.command =  cxi_command::ACK;
header.nbytes = htonl (stat_buf.st_size);
send_packet (client_sock, &header, sizeof header);
//send_packet (client_sock, output.c_str(), output.size());
}
void reply_rm(accepted_socket& client_sock,cxi_header&  header){
//std:: remove 
const char*  get_pipe = header.filename ;
//       struct stat stat_buf;
//       memset(&stat_buf, 0, sizeof stat_buf);

       int status = remove(get_pipe);
       if(status!= 0 ){
          outlog << get_pipe << ": " << strerror (errno) << endl;
      header.command = cxi_command::NAK;
      header.nbytes = htonl (errno);
     send_packet (client_sock, &header, sizeof header);
      return;
      }
header.command =  cxi_command::ACK;
send_packet (client_sock, &header, sizeof header);
}

void run_server (accepted_socket& client_sock) {
   outlog.execname (outlog.execname() + "*");
   outlog << "connected to " << to_string (client_sock) << endl;
   try {   
      for (;;) {
         cxi_header header; 
         recv_packet (client_sock, &header, sizeof header);
         DEBUGF ('h', "received header " << header);
         switch (header.command) {
            case cxi_command::LS: 
               reply_ls (client_sock, header);
               break;
            case cxi_command::GET:
               reply_get(client_sock, header);
               break; 
            case cxi_command::PUT:
               reply_put(client_sock, header);
               break;
            case cxi_command::RM:
               reply_rm(client_sock, header);
               break;
            default:
               outlog << "invalid client header:" << header << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      DEBUGF ('x', "caught cxi_exit");
   }
   throw cxi_exit();
}

void fork_cxiserver (server_socket& server, accepted_socket& accept) {
   pid_t pid = fork();
   if (pid == 0) { // child
      server.close();
      run_server (accept);
      throw cxi_exit();
   }else {
      accept.close();
      if (pid < 0) {
         outlog << "fork failed: " << strerror (errno) << endl;
      }else {
         outlog << "forked cxiserver pid " << pid << endl;
      }
   }
}


void reap_zombies() {
   for (;;) {
      int status;
      pid_t child = waitpid (-1, &status, WNOHANG);
      if (child <= 0) break;
      if (status != 0) {
         outlog << "child " << child
                << " exit " << (status >> 8)
                << " signal " << (status & 0x7F)
                << " core " << (status >> 7 & 1) << endl;
      }
   }
}

void signal_handler (int signal) {
   DEBUGF ('s', "signal_handler: caught " << strsignal (signal));
   reap_zombies();
}

void signal_action (int signal, void (*handler) (int)) {
   struct sigaction action;
   action.sa_handler = handler;
   sigfillset (&action.sa_mask);
   action.sa_flags = 0;
   int rc = sigaction (signal, &action, nullptr);
   if (rc < 0) outlog << "sigaction " << strsignal (signal)
                      << " failed: " << strerror (errno) << endl;
}



void usage() {
   cerr << "Usage: " << outlog.execname() << " port" << endl;
   throw cxi_exit();
}

in_port_t scan_options (int argc, char** argv) {
   for (;;) {
      int opt = getopt (argc, argv, "@:");
      if (opt == EOF) break;
      switch (opt) {
         case '@': debugflags::setflags (optarg);
                   break;
      }
   }
   if (argc - optind != 1) usage();
   return get_cxi_server_port (argv[optind]);
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   signal_action (SIGCHLD, signal_handler);
   try {
      in_port_t port = scan_options (argc, argv);
      server_socket listener (port);
      for (;;) {
         outlog << to_string (hostinfo()) << " accepting port "
             << to_string (port) << endl;
         accepted_socket client_sock;
         for (;;) {
            try {
               listener.accept (client_sock);
               break;
            }catch (socket_sys_error& error) {
               switch (error.sys_errno) {
                  case EINTR:
                     outlog << "listener.accept caught "
                         << strerror (EINTR) << endl;
                     break;
                  default:
                     throw;
               }
            }
         }
         outlog << "accepted " << to_string (client_sock) << endl;
         try {
            fork_cxiserver (listener, client_sock);
            reap_zombies();
         }catch (socket_error& error) {
            outlog << error.what() << endl;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      DEBUGF ('x', "caught cxi_exit");
   }
   return 0;
}

