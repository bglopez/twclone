#include <stdio.h>
#include <pthread.h>	// pthread ops
#include "listen.h"
#include "backend.h"
#include "common.h"
#include "universe.h"
#include "msgqueue.h"
#include "player_interaction.h"
#include "sysop_interaction.h"
#include "hashtable.h"
#include "planet.h"
#include "serveractions.h"
#include "shipinfo.h"
#include "config.h"



int main()
{
	pthread_t console_id, listen_id, backend_id;

	init_listen_thread(&listen_id);
	init_backend_thread(&backend_id);
	init_console();	
	// ------------------------------------------------------
	printf("main thread exiting...\n");
	return 0;
}
