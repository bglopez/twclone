/*
Copyright (C) 2000 Jason C. Garcowski(jcg5@po.cwru.edu), 
                   Ryan Glasnapp(rglasnap@nmt.edu)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
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
struct config *configdata;
time_t *starttime, *curtime;

int main(int argc, char *argv[])
{
  int sockid, port = DEFAULT_PORT, msgidin, msgidout, senderid;
  pthread_t threadid;
  struct connectinfo *threadinfo = (struct connectinfo *) malloc(sizeof(struct connectinfo));
  struct sockaddr_in serv_sockaddr;
  struct msgcommand data;
  char buffer[BUFF_SIZE];

  time(starttime);
  //reading the port to run on from the command line
  if (argc > 1)
    {
      //change the string to a long
      port = strtoul(argv[1], NULL, 10);
      if (port == 0) //if it wasn't possible to change to an int
	{
	  //quit, and tell the user how to use us
	  printf("usage:  %s [port_num]\n", argv[0]);
	  exit(-1);
	}
    }

  init_hash_table(symbols, HASH_LENGTH);

  printf("initializing configuration data from 'config.data'...");
  fflush(stdout);
  init_config("config.data");
  printf(" Done\n");
  
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
  
  /*looks like maybe I shouldn't do this
    printf("Verify Universe...");
    fflush(stdout);
    if (verify_universe(sectors, sectorcount) < 0)
    {
    printf(" Failed, exiting!\n");
    exit(-1);
    }
    printf(" Done\n");
  */

  printf("Initializing random number generator...");
  srand((int)time(NULL));
  printf(" Done\n");

  printf("Initializing message queues...");
  msgidin = init_msgqueue();
  msgidout = init_msgqueue();
  printf(" Done\n");

  printf("Creating sockets....");
  fflush(stdout);
  sockid = init_sockaddr(port, &serv_sockaddr);
  printf(" Listening on port %d!\n", port);

  threadinfo->sockid = sockid;
  threadinfo->msgidin = msgidin;
  threadinfo->msgidout = msgidout;
  if (pthread_create(&threadid, NULL, makeplayerthreads, (void *) threadinfo) != 0)
    {
      perror("Unable to Create Listening Thread");
      exit(-1);
    }
  printf("Accepting connections!\n");

  threadinfo = (struct connectinfo *)malloc(sizeof(struct connectinfo));
  threadinfo->msgidin = msgidin;
  threadinfo->msgidout = msgidout;
  if (pthread_create(&threadid, NULL, getsysopcommands, (void *) threadinfo) != 0)
    {
      perror("Unable to Create Sysop Thread");
      exit(-1);
    }
  printf("Accepting Sysop commands\n");

  //process the commands from the threads
  senderid = getdata(msgidin, &data, 0);
  while(data.command != ct_quit || senderid != threadid)  //Main game loop
    {
      time(curtime);
      if ((curtime - starttime)% configdata->autosave * 60) //Autosave
	      saveall();
      if ((curtime - starttime)% 3600 == 0)
	      ;//Regen turns;
      if ((curtime - starttime)% 86400 == 0)
	      ;//Regen fractional turns leftover
      if ((curtime - starttime)% configdata->processinterval == 0)
      {
	//Process real time stuff?
      }
      if ((curtime - starttime)% 3*configdata->processinterval == 0)
      {
	//Alien movement here.
      }
      processcommand(buffer, &data);
      sendmsg(msgidout, buffer, senderid);
      senderid = getdata(msgidin, &data, 0);
    }

  //when we're done, clean up the msg queues
  if (fork() == 0)
    {
      sprintf(buffer, "%d", msgidin);
      fprintf(stderr, "Killing message queue with id %s...", buffer);
      if (execlp("ipcrm", "ipcrm", "msg", buffer, NULL) < 0)
	{
	  perror("Unable to exec: ");
	  printf("Please run 'ipcrm msg %d'\n", msgidin);
	}
    }
  else
    {
      sprintf(buffer, "%d", msgidout);
      fprintf(stderr, "Killing message queue with id %s...", buffer);
      if (execlp("ipcrm", "ipcrm", "msg", buffer, NULL) < 0)
	{
	  perror("Unable to exec: ");
	  printf("Please run 'ipcrm msg %d'\n", msgidout);
	}
    }
  
  return 0;
}