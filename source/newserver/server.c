#include <stdio.h>
#include <pthread.h>	// pthread ops
#include "listen.h"
#include "backend.h"
#include <signal.h>
#include <unistd.h>

void handle_signal(int sig);

static pthread_t listen_id, backend_id;

int main(){
	init_listen_thread(&listen_id);
	init_backend_thread(&backend_id);
	signal(SIGINT, handle_signal);
	init_console();	
	// ------------------------------------------------------
	printf("main thread exiting...\n");
	return 0;
}

void handle_signal(int sig){
        switch(sig){
        case SIGHUP: printf("HUP!\n"); break;
        case SIGINT:
		printf("caught SIGINT, cleaning up...\n"); 
		printf("going to send kill signals to %d %d\n", 
			listen_id, backend_id); 
		exit(0);
        default: printf("hmnm!\n"); break;
        }
}

