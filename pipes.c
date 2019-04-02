#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>

int main(int argc, char **argv){
	
	int pipeEnd[2];
	pipe(pipeEnd);
	
	pid_t childpid = fork();
	if(childpid == -1){
		printf("Forking error\n");
		return 1;
	}
	
	#define MSG_BUFSIZE 13
	char msg[MSG_BUFSIZE];
	if(childpid == 0){ // Is child
		close(pipeEnd[0]);
		int nextNum = 1;
		srand(time(NULL));
		for(int i = 0; i<10; i++){
			sprintf(msg, "%d", nextNum);
			printf("Sending: %s\n", msg);
			write(pipeEnd[1], msg, MSG_BUFSIZE);
			nextNum += rand() % 8;
		}
		printf("Sending: 0\n");
		write(pipeEnd[1], "0", 2);
		close(pipeEnd[1]);
		printf("Encerrando escritor\n");

	}else{ // Is parent
		close(pipeEnd[1]);
		int size;
		do {
			size = read(pipeEnd[0], msg, MSG_BUFSIZE);
			printf("\tRecv: %s (%d)\n", msg, size);
			int value = atoi(msg);
			// Check primality
			
			int isPrime = value >= 1?1:0;
			for(int i = 2; i<= ceil(sqrt(value)); i++){
				if((value % i) == 0){
					isPrime = 0;
					break;
				}
			}
			
			printf("\t%d %sé primo\n", value, isPrime?"":"nao ");
			
			
		} while(msg[0] != '0' && size > 0);
		close(pipeEnd[0]);
		printf("Encerrando leitor\n");
	}
	
	return 0;
}

