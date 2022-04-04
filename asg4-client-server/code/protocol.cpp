// $Id: protocol.cpp,v 1.17 2021-05-18 01:32:29-07 - - $

#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
using namespace std;

#include "protocol.h"

string to_string (cxi_command command) {
   switch (command) {
      case cxi_command::ERROR  : return "ERROR"  ;
      case cxi_command::EXIT   : return "EXIT"   ;
      case cxi_command::GET    : return "GET"    ;
      case cxi_command::HELP   : return "HELP"   ;
      case cxi_command::LS     : return "LS"     ;
      case cxi_command::PUT    : return "PUT"    ;
      case cxi_command::RM     : return "RM"     ;
      case cxi_command::FILEOUT: return "FILEOUT";
      case cxi_command::LSOUT  : return "LSOUT"  ;
      case cxi_command::ACK    : return "ACK"    ;
      case cxi_command::NAK    : return "NAK"    ;
      default                  : return "????"   ;
   };
}


void send_packet (base_socket& socket,
                  const void* buffer, size_t bufsize) {
   const char* bufptr = static_cast<const char*> (buffer);
   ssize_t ntosend = bufsize;
   do {
      ssize_t nbytes = socket.send (bufptr, ntosend);
      if (nbytes < 0) throw socket_sys_error (to_string (socket));
      bufptr += nbytes;
      ntosend -= nbytes;
   }while (ntosend > 0);
}

void recv_packet (base_socket& socket, void* buffer, size_t bufsize) {
   char* bufptr = static_cast<char*> (buffer);
   ssize_t ntorecv = bufsize;
   do {
      ssize_t nbytes = socket.recv (bufptr, ntorecv);
      if (nbytes < 0) throw socket_sys_error (to_string (socket));
      if (nbytes == 0) throw socket_error (to_string (socket)
                                           + " is closed");
      bufptr += nbytes;
      ntorecv -= nbytes;
   }while (ntorecv > 0);
}


string to_hex32_string (uint32_t num) {
   ostringstream stream;
   stream << "0x" << hex << uppercase << setfill('0') << setw(8) << num;
   return stream.str();
}

ostream& operator<< (ostream& out, const cxi_header& header) {
   constexpr size_t WARNING_NBYTES = 1<<20;
   uint32_t nbytes = htonl (header.nbytes);
   if (nbytes > WARNING_NBYTES) {
      out << "WARNING: Payload nbytes " << nbytes << " > "
          << WARNING_NBYTES << endl;
   }
   return out << "{" << to_hex32_string (header.nbytes) << ':'
              << header.nbytes << ':' << ntohl (header.nbytes) << ","
              << unsigned (header.command)
              << "(" << to_string (header.command) << "),\""
              << header.filename << "\"}";
}    

in_port_t get_cxi_server_port (const string& port_arg) {
   auto error = socket_error (port_arg + ": invalid port number");
   try {
      constexpr int min = numeric_limits<in_port_t>::min();
      constexpr int max = numeric_limits<in_port_t>::max();
      int port = stoi (port_arg);
      if (port < min or port > max) throw error;
      return port;
   }catch (invalid_argument&) { // thrown by stoi
      throw error;
   }catch (out_of_range&) { // thrown by stoi
      throw error;
   }
}
     
