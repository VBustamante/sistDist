#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>

int main(int argc, char **argv){
	
	if(argc != 2){
		printf("Missing sequence size. run \"pipes $seqsize\"\n"
		"$seqsize: Quantity of numbers to generate\n");
		return 1;
	}
	int seqsize = atoi(argv[1]);

	
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
		for(int i = 0; i<=seqsize; i++){
			sprintf(msg, "%d", i == seqsize?0:nextNum);
			printf("Sending: %s\n", msg);
			write(pipeEnd[1], msg, MSG_BUFSIZE);
			nextNum += rand() % 8;
		}
		close(pipeEnd[1]);
		printf("Encerrando escritor\n");

	}else{ // Is parent
		close(pipeEnd[1]);
		int value;
		do {
			int size = read(pipeEnd[0], msg, MSG_BUFSIZE);
			printf("\tRecv: %s (%d)\n", msg, size);
			value = atoi(msg);
			// Check primality
			
			int isPrime = value >= 1?1:0;
			for(int i = 2; i<= ceil(sqrt(value)); i++){
				if((value % i) == 0){
					isPrime = 0;
					break;
				}
			}
			
			printf("\t%d %sé primo\n", value, isPrime?"":"nao ");
			
			
		} while(value);
		close(pipeEnd[0]);
		printf("Encerrando leitor\n");
	}
	
	return 0;
}

