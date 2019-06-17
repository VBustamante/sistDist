//
// Created by Victor Bustamante on 2019-06-15.
//

#include "Networker.h"


#define BACKLOG 10   // how many pending connections queue will hold


Networker::Networker(const char *arg_port) {

  this->port = arg_port;

  struct addrinfo *servinfo;
  {
    struct addrinfo hint{};
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC; // Ipv4 or v6
    hint.ai_socktype = SOCK_STREAM; // TCP
    hint.ai_flags = AI_PASSIVE;

    int rv;
    if((rv = getaddrinfo(nullptr, port, &hint, &servinfo)) != 0){
      printf("getaddrinfo: %s\n", gai_strerror(rv));
      return;
    }
  }

  {
    struct addrinfo *p;
    for(p = servinfo; p != nullptr; p = p->ai_next){
      if((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) continue;

      int yes = 1;
      if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
        printf("Error setting sock options\n");
        return;
      }

      struct timeval tv{};
      tv.tv_sec = TIMEOUT;
      tv.tv_usec = 0;
      setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);



      if(bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1){
        close(socket_fd);
        continue;
      }

      break;
    }

    if(p == nullptr){
      printf("Failed to bind\n");
      return;
    }
  }
  freeaddrinfo(servinfo);

  if(listen(socket_fd, BACKLOG) == -1){
    printf("Failed to bind\n");
    return;
  }

  printf("Server up on %s\n", port);
}

void Networker::accept_msg() {
  struct sockaddr_storage remote_addr{};

  socklen_t sin_size;
  socket_conn_fd = accept(socket_fd, (struct sockaddr *)&remote_addr, &sin_size);
  if(socket_conn_fd == -1){
    printf("Error connecting to client\n");
    return;
  }
}

void Networker::get_msg(char *message) {
  if(read(socket_conn_fd, message, MSG_BUFSIZE) != MSG_BUFSIZE){
    message[0]=(char) RequestType::size;
    message[1]= 0;
  }
}

void Networker::respond_msg(const char *message) {
  if(send(socket_conn_fd, message, MSG_BUFSIZE, 0) == -1) printf("Error reading request\n");
}

void Networker::close_msg() {
  close(socket_conn_fd);
}

Networker::~Networker() {
  close(socket_fd);
}

void Networker::send_msg(const char* host, const char* port, const char* message) {
  struct addrinfo *servinfo;
  {
    struct addrinfo hints{};
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int rv;
    if((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0){
      printf("getaddrinfo: %s\n", gai_strerror(rv));
      return;
    }

  }

  int socket_fd = 0;
  {
    struct addrinfo *p;
    for(p = servinfo; p != nullptr; p = p->ai_next){
      if((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) continue;

      if(connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1){
        close(socket_fd);
        continue;
      }

      break;
    }

    if(p == nullptr){

      std::string RequestTypeNames[] = {
          "ELEICAO",
          "OK",
          "LIDER",
          "VIVO",
          "VIVO_OK"
      };

      printf("Failed to send %s to %s\n", RequestTypeNames[(int) message[0]].c_str(), port);
      return;
    }
  }
  freeaddrinfo(servinfo);

  send(socket_fd, message, MSG_BUFSIZE, 0);

  close(socket_fd);
}
