#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>

int main(int argc, char **argv){
	// Start listen socket
	
	#define MSG_BUFSIZE 13
	char msg[MSG_BUFSIZE];
	int nextNum = 1;
	srand(time(NULL));
	for(int i = 0; i<10; i++){
		sprintf(msg, "%d", nextNum);
		printf("Sending: %s\n", msg);
//		write(pipeEnd[1], msg, strlen(msg) + 1);
		sleep(1);
		nextNum += rand() % 8;
	}
	printf("Sending: 0\n");
//	write(pipeEnd[1], "0\0", 2);
	printf("Encerrando escritor\n");

	
	return 0;
}

