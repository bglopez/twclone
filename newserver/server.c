#include <stdio.h>
#include <pthread.h>	// pthread ops
#include "listen.h"
#include "backend.h"

int main(){
	pthread_t console_id, listen_id, backend_id;
	init_listen_thread(&listen_id);
	init_backend_thread(&backend_id);
	init_console();	
	// ------------------------------------------------------
	printf("main thread exiting...\n");
	return 0;
}

