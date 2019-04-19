#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <math.h>
#include <time.h>

#define PORT "8080"

#define MAXDATASIZE 100

int main(int argc, char **argv){
	if(argc != 2){
		printf("Missing receiver host. run \"pipes $host\"\n"
		"$host: Receiver's IP\n");
		return 1;
	}
	char *recHost = argv[1];


	//Start socket
	
	
	struct addrinfo *servinfo;
	{
		struct addrinfo hints;
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		
		int rv;
		if((rv = getaddrinfo(recHost, PORT, &hints, &servinfo)) != 0){
			printf("getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}
		
	}

	int socket_fd;
	{
		struct addrinfo *p;
		for(p = servinfo; p != NULL; p = p->ai_next){
			if((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) continue;
						
			if(connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1){
				close(socket_fd);
				continue;
			}
			
			break;
		}
		
		if(p == NULL){
			printf("Failed to connect\n");
			return 1;
		}
	}
	freeaddrinfo(servinfo);

	
	
	#define MSG_BUFSIZE 13
	char msg[MSG_BUFSIZE];
	do {
		int size = recv(socket_fd , msg, MSG_BUFSIZE, 0);
		if(size == -1){
			printf("Read error\n");
			break;
		}
		printf("Recv: %s (%d)\n", msg, size);
		int value = atoi(msg);
		
		if(!value) break;
		
		// Check primality		
		int isPrime = value >= 1?1:0;
		for(int i = 2; i<= ceil(sqrt(value)); i++){
			if((value % i) == 0){
				isPrime = 0;
				break;
			}
		}
		
		printf("%d %sé primo\n", value, isPrime?"":"nao ");
		sprintf(msg, "%s", isPrime?"1":"0");
		send(socket_fd, msg, strlen(msg), 0);
		
	} while(1);
	close(socket_fd);
	printf("Closing Reader\n");
	
	return 0;
}

