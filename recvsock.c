#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>

int main(int argc, char **argv){
	//Start socket
	#define MSG_BUFSIZE 13
	char msg[MSG_BUFSIZE];
	do {
//		int size = read(pipeEnd[0], msg, MSG_BUFSIZE);
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
		
		
	} while(msg[0] != '0');
//	close(pipeEnd[0]);
	printf("Encerrando leitor\n");
	
	return 0;
}

