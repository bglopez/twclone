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


/* Modification History **
**************************
** 
** LAST MODIFICATION DATE: 22 June 2002
** Author: Rick Dearman
** 1) Modified arguments to use getopt
**
*/


#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
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
#include "maint.h"

struct sector **sectors;
int sectorcount;
struct list *symbols[HASH_LENGTH];
struct player *players[MAX_PLAYERS];
struct ship *ships[MAX_SHIPS];
struct port *ports[MAX_PORTS];
struct config *configdata;
time_t *timeptr;
time_t starttime;

int
main (int argc, char *argv[])
{
  int c;
  int sockid, port, msgidin, msgidout, senderid;
  pthread_t threadid;
  struct connectinfo *threadinfo =
    (struct connectinfo *) malloc (sizeof (struct connectinfo));
  struct sockaddr_in serv_sockaddr;
  struct msgcommand data;
  char buffer[BUFF_SIZE];


  char *usageinfo =
    "Usage: server [options]\n    Options:-p < integer >\n    the port number the server will listen on (Default 1234) \n";
  port = DEFAULT_PORT;
  opterr = 0;

  while ((c = getopt (argc, argv, "p:")) != -1)
    {
      switch (c)
	{
	case 'p':
	  port = strtoul (optarg, NULL, 10);
	  break;
	case '?':
	  if (isprint (optopt))
	    fprintf (stderr, "Unknown option `-%c'.\n\n%s", optopt,
		     usageinfo);
	  else
	    fprintf (stderr,
		     "Unknown option character `\\x%x'.\n\n%s",
		     optopt, usageinfo);
	  return 1;
	default:
	  abort ();
	}
    }



  starttime = time (timeptr);

  init_hash_table (symbols, HASH_LENGTH);

  printf ("initializing configuration data from 'config.data'...");
  fflush (stdout);
  init_config ("config.data");
  printf (" Done\n");

  printf ("initializing the universe from '%s'...", "universe.data");
  fflush (stdout);
  sectorcount = init_universe ("universe.data", &sectors);
  printf (" Done\n");

  printf ("Reading in planet information from 'planets.data'...\n");
  fflush (stdout);
  init_planets ("planets.data", sectors);
  printf ("... Done!\n");

  init_shiptypeinfo ();

  printf ("Reading in ship information from 'ships.data'...");
  fflush (stdout);
  init_shipinfo ("ships.data");
  printf (" Done!\n");

  printf ("Reading in player information from 'players.data'...");
  fflush (stdout);
  init_playerinfo ("players.data");
  printf (" Done!\n");

  printf ("Reading in port information from 'ports.data'...");
  fflush (stdout);
  init_portinfo ("ports.data");
  printf (" Done!\n");

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

  printf ("Initializing random number generator...");
  srand ((int) time (NULL));
  printf (" Done\n");

  printf ("Initializing message queues...");
  msgidin = init_msgqueue ();
  msgidout = init_msgqueue ();
  printf (" Done\n");

  printf ("Creating sockets....");
  fflush (stdout);
  sockid = init_sockaddr (port, &serv_sockaddr);
  printf (" Listening on port %d!\n", port);

  threadinfo->sockid = sockid;
  threadinfo->msgidin = msgidin;
  threadinfo->msgidout = msgidout;
  if (pthread_create (&threadid, NULL, makeplayerthreads, (void *) threadinfo)
      != 0)
    {
      perror ("Unable to Create Listening Thread");
      exit (-1);
    }
  printf ("Accepting connections!\n");

  printf ("Initializing background maintenance...");
  fflush (stdout);
  threadinfo = (struct connectinfo *) malloc (sizeof (struct connectinfo));
  threadinfo->msgidin = msgidin;
  threadinfo->msgidout = msgidout;
  if (pthread_create (&threadid, NULL, background_maint, (void *) threadinfo)
      != 0)
    {
      perror ("Unable to Create Backgroud Thread");
      exit (-1);
    }
  printf ("Done!\n");

  threadinfo = (struct connectinfo *) malloc (sizeof (struct connectinfo));
  threadinfo->msgidin = msgidin;
  threadinfo->msgidout = msgidout;
  if (pthread_create (&threadid, NULL, getsysopcommands, (void *) threadinfo)
      != 0)
    {
      perror ("Unable to Create Sysop Thread");
      exit (-1);
    }
  printf ("Accepting Sysop commands\n");

  //process the commands from the threads
  senderid = getdata (msgidin, &data, 0);
  while (data.command != ct_quit || senderid != threadid)	//Main game loop
    {
      processcommand (buffer, &data);
      sendmsg (msgidout, buffer, senderid);
      senderid = getdata (msgidin, &data, 0);
    }
  saveallports ();

  //when we're done, clean up the msg queues
  if (fork () == 0)
    {
      sprintf (buffer, "%d", msgidin);
      fprintf (stderr, "Killing message queue with id %s...", buffer);
      if (execlp ("ipcrm", "ipcrm", "msg", buffer, NULL) < 0)
	{
	  perror ("Unable to exec: ");
	  printf ("Please run 'ipcrm msg %d'\n", msgidin);
	}
    }
  else
    {
      sprintf (buffer, "%d", msgidout);
      fprintf (stderr, "Killing message queue with id %s...", buffer);
      if (execlp ("ipcrm", "ipcrm", "msg", buffer, NULL) < 0)
	{
	  perror ("Unable to exec: ");
	  printf ("Please run 'ipcrm msg %d'\n", msgidout);
	}
    }

  return 0;
}
