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
#include "baseconfig.h"
#include "maint.h"

struct sector **sectors;
int sectorcount;
struct list *symbols[HASH_LENGTH];
struct Player **players;
struct ship **ships;
Port **ports;
struct config *configdata;
struct sp_shipinfo **shiptypes;
struct node **nodes;

time_t *timeptr;
time_t starttime;
int WARP_WAIT = 1;
int GAMEON = 1;

int main (int argc, char *argv[])
{
    int c;
    int sockid, port, msgidin, msgidout, senderid;
    pthread_t threadid;
    struct connectinfo *threadinfo =
                    (struct connectinfo *) malloc (sizeof (struct connectinfo));
    struct sockaddr_in serv_sockaddr;
    struct msgcommand data;
	 int elapsedtime, heartbeat;
    char buffer[BUFF_SIZE];
	 struct timeval *lasttime, *curtime, *swap;
	 struct b_maint *lmaint;
	 
	 lasttime = (struct timeval *)malloc(sizeof(struct timeval));
	 curtime = (struct timeval *)malloc(sizeof(struct timeval));
	 lmaint = (struct b_maint *)malloc(sizeof(struct b_maint));

	 lmaint->regen = -1;
	 lmaint->day = -1;

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
    init_config ("./config.data");
    printf (" Done!\n");

	 printf("initializing planet type data from 'planettypes.data' ...");
	 fflush(stdout);
	 init_planetinfo("./planettypes.data");
	 printf(" Done!\n");

	 printf("initializing ship type data from 'shiptypes.data'...");
    init_shiptypeinfo("./shiptypes.data");
	 printf("... Done!\n");
	 
    printf ("initializing the universe from '%s'...", "universe.data");
    fflush (stdout);
    sectorcount = init_universe ("./universe.data", &sectors);
    printf (" Done!\n");

    printf ("Reading in planet information from 'planets.data'...\n");
    fflush (stdout);
    init_planets ("./planets.data");
    printf ("... Done!\n");

    printf ("Reading in ship information from 'ships.data'...");
    fflush (stdout);
    init_shipinfo ("./ships.data");
    printf (" Done!\n");

    printf ("Reading in player information from 'players.data'...");
    fflush (stdout);
    init_playerinfo ("./players.data");
    printf (" Done!\n");

    printf ("Reading in port information from 'ports.data'...");
    fflush (stdout);
    ports = new Port("./ports.data", configdata);
    printf (" Done!\n");

	 printf("Configuring node information...");
	 fflush(stdout);
	 init_nodes(sectorcount);
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

    printf ("Initializing random number generator...");
    srand ((int) time (NULL));
    printf (" Done\n");

    printf ("Initializing message queues...");
    msgidin = init_msgqueue ();
    msgidout = init_msgqueue ();
    printf (" Done\n");
    printf("Cleaning up any old message queues...");
    clean_msgqueues(msgidin, msgidout, "msgqueue.lock");
    printf(" Done\n");


    printf ("Creating sockets....");
    fflush (stdout);
    sockid = init_sockaddr (port, &serv_sockaddr);
    printf (" Listening on port %d!\n", port);

    threadinfo->sockid = sockid;
    threadinfo->msgidin = msgidin;
    threadinfo->msgidout = msgidout;

    printf ("Accepting connections!\n");

/*    printf ("Initializing background maintenance...");
    fflush (stdout);
    printf ("Done!\n");*/

	 gettimeofday(lasttime, 0);
	 do
	 {
	 	handle_sockets(sockid, msgidin, msgidout);

		gettimeofday(curtime, 0);
  		do
		{
		senderid = getdata (msgidin, &data, 0);
		if (senderid > 0)
		{
     	  processcommand (buffer, &data);
  		  sendmesg (msgidout, buffer, senderid);
		}
		}while(senderid > 0);

		background_maint(lmaint);
		
		swap = curtime;
		curtime = lasttime;
		lasttime = swap;
		//maintenance.
		if (GAMEON==0)
			break;
	 }
	 while(senderid != -1);

	 printf("\nSaving all ports ...");
	 fflush(stdout);
    saveallports ("ports.data");
	 printf("Done!");
	 printf("\nSaving planets ...");
	 fflush(stdout);
	 saveplanets("planets.data");
	 printf("Done!");
	 printf("\nSaving ship types ...");
	 fflush(stdout);
	 saveshiptypeinfo("shiptypes.data");
	 printf("Done!");
	 printf("\nSaving planet types ...");
	 fflush(stdout);
	 save_planetinfo("planettypes.data");
	 printf("Done!");
	 printf("\nSaving configuration data ...");
	 fflush(stdout);
	 saveconfig("config.data");
	 printf("Done!");
	 fflush(stdout);

	 //when we're done, clean up the msg queues
    c=0;
    sprintf (buffer, "%d", msgidin);
    fprintf (stderr, "\nKilling message queue with id %s...", buffer);
    if(msgctl(msgidin, IPC_RMID, NULL)==-1)
    {
        if (execlp ("ipcrm", "ipcrm", "msg", buffer, NULL) < 0)
        {
           perror ("Unable to exec: ");
           printf ("Please run 'ipcrm msg %d'\n", msgidin);
        }
    }
    else 
	 {
         c++;
    }
    sprintf (buffer, "%d", msgidout);
    fprintf (stderr, "\nKilling message queue with id %s...", buffer);
    if(msgctl(msgidout, IPC_RMID, NULL)==-1) 
	 {
       if (execlp ("ipcrm", "ipcrm", "msg", buffer, NULL) < 0)
       {
           perror ("Unable to exec: ");
           printf ("Please run 'ipcrm msg %d'\n", msgidout);
       }
    }
    else 
	 {
         c++;
         fprintf(stderr, "\n");
    }
    if(c<2 || unlink("./msgqueue.lock")==-1)
         printf("\nPlease run 'rm msgqueue.lock'\n");
    return 0;
}
