#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>


int running = 1;
static void handleSIGUSR1(int signum){
	printf("Handle SIGUSR1\n");
}

static void handleSIGUSR2(int signum){
	printf("Handle SIGUSR2\n");
}

static void handleSIGALRM(int signum){
	running = 0;
	printf("Handle SIGALRM\n");
}

#define WAIT_BUSY 0
#define WAIT_BLOCK 1

int main(int argc, char **argv){
	if(argc != 2){
		printf("Missing wait type. run \"recvsig $wait\"\n"
		"$wait: busy = %d, blocking = %d\n", WAIT_BUSY, WAIT_BLOCK);
		return 1;
	}

	printf("Setting up handlers\n");	
	struct sigaction saALRM;
	sigemptyset(&saALRM.sa_mask);
	saALRM.sa_flags = SA_RESTART;
	saALRM.sa_handler = handleSIGALRM;
	sigaction(SIGALRM, &saALRM, NULL);
	printf("Did SIGALRM\n");
	
	struct sigaction saUSR1;
	sigemptyset(&saUSR1.sa_mask);
	saUSR1.sa_flags = SA_RESTART;
	saUSR1.sa_handler = handleSIGUSR1;
	sigaction(SIGUSR1, &saUSR1, NULL);
	printf("Did SIGUSR1\n");
	
	struct sigaction saUSR2;
	sigemptyset(&saUSR2.sa_mask);
	saUSR2.sa_flags = SA_RESTART;
	saUSR2.sa_handler = handleSIGUSR2;
	sigaction(SIGUSR2, &saUSR2, NULL);
	printf("Did SIGUSR2\n");
	
	int wait = atoi(argv[1]);

	printf("Starting loop (pid = %d)\n", getpid());	
	
	if(wait == WAIT_BUSY) while(running);
	else while(running) pause();	
	
	printf("Exiting now\n");
	return 0;
}
