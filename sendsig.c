#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char **argv){
	if(argc != 3){
		printf("Missing parameters. run \"sendsig $pid $sigid\"\n"
		"Signal codes:\n"
		"SIGINT: %d\n"
		"SIGUSR1: %d\n"
		"SIGUSR2: %d\n" 
		"SIGALRM: %d\n",
		SIGINT, SIGUSR1, SIGUSR2, SIGALRM);
		return 1;
	}
	
	int pid, signal;
	
	pid = atoi(argv[1]);
	if(pid == 0){
		printf("Invalid pid (unreadable or 0)\n");
		return 1;
	}


	signal = atoi(argv[2]);

	if(kill(pid, signal) != 0){
		printf("Error sending signal\n");
		return 1;		
	}

	return 0;
}
