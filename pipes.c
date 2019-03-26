#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char **argv){
	
	int pipeEnd[2];
	pipe(pipeEnd);
	
	pid_t childpid = fork();
	if(childpid == -1){
		printf("Forking error\n");
		return 1;
	}
	
	char msg[20];
	if(childpid == 0){ // Is child
		write(pipeEnd[1], "Hello\n", 7);
	}else{ // Is parent
		read(pipeEnd[0], msg, 20);
		printf("Recv: %s", msg);
	}
	
	return 0;
}

