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


enum class RequestType: unsigned char{
    ELEICAO,
    OK,
    LIDER,
    VIVO,
    VIVO_OK
};


#define MSG_BUFSIZE 13

class Networker {
  int socket_fd = 0, socket_respose_fd = 0;
  const char * port;

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
};


#endif //PARTE3_NETWORKER_H
