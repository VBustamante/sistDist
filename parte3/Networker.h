//
// Created by Victor Bustamante on 2019-06-15.
//

#ifndef PARTE3_NETWORKER_H
#define PARTE3_NETWORKER_H

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>


enum class RequestType: unsigned char{
  ELEICAO,
  OK,
  LIDER,
  VIVO,
  VIVO_OK,
  size
};

std::string const& RequestTypeNames(unsigned char index);

#define MSG_BUFSIZE 2
#define TIMEOUT 2

class Networker {
  int socket_fd = 0, socket_conn_fd = 0;
  const char *port;

public:
  explicit Networker(const char *port);

  virtual ~Networker();

  // Open incoming connection
  void accept_msg();

  // Copy incoming data to a string
  void get_msg(char *message);

  // Send something up the socket
  void respond_msg(const char *message);

  // Close connection
  void close_msg();

  // Open proactive socket to send data
  static void send_msg(const char* host, const char* port, const char* message);

  static void send_msg(const char* port, const char* message) { send_msg("localhost", port, message); }

  void respond_msg(RequestType type, int id){
    char msg[MSG_BUFSIZE] = {(char) type, (char) id};
    respond_msg(msg);
  }

  static void send_msg(const char* port, RequestType type, int id) {
    char msg[MSG_BUFSIZE] = {(char) type, (char) id};
    send_msg(port, msg);
  }

};


#endif //PARTE3_NETWORKER_H
