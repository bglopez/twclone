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

struct sector **sectors;
int sectorcount;
struct list *symbols[HASH_LENGTH];
struct player *players[MAX_PLAYERS];
struct ship *ships[MAX_SHIPS];
struct port *ports[MAX_PORTS];



int main()
{
	
  pthread_t console_id, listen_id, backend_id;

  init_hash_table(symbols, HASH_LENGTH);
  //Load universe stuff into memory.
  printf("initializing the universe from '%s'...", "universe.data");
  fflush(stdout);
  sectorcount = init_universe("universe.data", &sectors);
  printf(" Done\n");

  printf("Reading in planet information from 'planets.data'...\n");
  fflush(stdout);
  init_planets("planets.data", sectors);
  printf("... Done!\n");

  init_shiptypeinfo();

  printf("Reading in ship information from 'ships.data'...");
  fflush(stdout);
  init_shipinfo("ships.data");
  printf(" Done!\n");

  printf("Reading in player information from 'players.data'...");
  fflush(stdout);
  init_playerinfo("players.data");
  printf(" Done!\n");
  
  printf("Reading in port informatino from 'ports.data'...");
  fflush(stdout);
  init_portinfo("ports.data");
  printf(" Done!\n");
 	
  //Thread stuff starts here
  init_listen_thread(&listen_id);
  init_backend_thread(&backend_id);
  init_console();	
  // ------------------------------------------------------
  printf("main thread exiting...\n");
  return 0;
}
