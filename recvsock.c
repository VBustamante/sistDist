#include <stdio.h>
#include <stdlib.h>
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
#include <math.h>
#include <time.h>

#define PORT "8080"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

#define MSG_BUFSIZE 13


int main(int argc, char **argv){
	// Start listen socket
	
	struct addrinfo *servinfo;
	{
		struct addrinfo hint;
		memset(&hint, 0, sizeof(hint));
		hint.ai_family = AF_UNSPEC; // Ipv4 or v6
		hint.ai_socktype = SOCK_STREAM; // TCP
		hint.ai_flags = AI_PASSIVE;
		
		int rv;
		if((rv = getaddrinfo(NULL, PORT, &hint, &servinfo)) != 0){
			printf("getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}
	}
	
	int socket_fd;
	{
		struct addrinfo *p;
		for(p = servinfo; p != NULL; p = p->ai_next){
			if((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) continue;
			
			int yes = 1;
			if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
				printf("Error setting sock options\n");
				return 1;
			}
			
			if(bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1){
				close(socket_fd);
				continue;
			}
			
			break;
		}
		
		if(p == NULL){
			printf("Failed to bind\n");
			return 1;
		}
	}
	freeaddrinfo(servinfo);
	
	if(listen(socket_fd, BACKLOG) == -1){
		printf("Failed to bind\n");
		return 1;
	}
	
	printf("Server up\n");
	
	printf("Waiting connection\n");
	struct sockaddr_storage remote_addr;
	
	socklen_t sin_size;
	int socket_res_fd = accept(socket_fd, (struct sockaddr *)&remote_addr, &sin_size);
	if(socket_res_fd == -1){
		printf("Error connecting to client\n");
		return 1;
	}

	
	char msg[MSG_BUFSIZE];
	int printNum = 1;
	srand(time(NULL));
	int seqSize = 5;
	for(int i = 0; i<seqSize; i++){
		sprintf(msg, "%d", printNum);
		printf("Sending: %s\n", msg);
		if(send(socket_res_fd, msg, strlen(msg) + 1, 0) == -1) printf("Error reading request\n");

		read(socket_res_fd, msg, MSG_BUFSIZE);
		
		printf("%d %sé primo\n", printNum, msg[0] == '0'? "nao ":"");
		
		printNum += (rand() % 8) + 1;
	}
	send(socket_res_fd, "0", strlen(msg) + 1, 0);
	close(socket_res_fd);
	close(socket_fd);
	printf("Closing Writer\n");

	
	return 0;
}

