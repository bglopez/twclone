/*
** Copyright (c) 2002 Rick Dearman <rick@ricken.demon.co.uk>
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** $Author: eryndil $
** $Date: 2002-07-17 14:24:36 $
** $RCSfile: network.c,v $
** $Revision: 1.1.1.1 $
** $State: Exp $
**
** Description: This program is used to listen on ports 
**		described in the ini file and then launch
**              the correct game based on that information.
**
**	This file contains the network related functions.
*/


/*******************************
 ***  Includes
 ******************************/

#include "server.h"
#include <nspr.h>
#include <string.h>
#include <prmem.h>

/*******************************
 ***  Global Vars.
 ******************************/

#define RECV_FLAGS 0
#define BUFFER_SIZE 4096
char *MyIPAddress;
struct gamenode *nodeOne = NULL;
struct gamenode *newgame;

/*********************************
 ***  Externally defined fuctions
 *********************************/
extern void print_err_msg (PRErrorCode eTheError);
extern char *save_string (char *string);
extern void outputXML (PRFileDesc * f);


/*********************************
 ***  Locally defined fucntions
 ********************************/

char *read_line (PRFileDesc * s);
void ServiceThread (void *pData);


void
RunCmd (PRFileDesc * s, char *arg)
{
  /* intentanally blank */
}

/* This function starts the server threads 
** and listens on the port
** which was defined in the ini file.
*/


void
server (void)
{
  PRNetAddr local_addr_sa;
  PRNetAddr remote_addr;
  PRFileDesc *listen_socket;
  PRFileDesc *new_conn_socket;
  char buf2[256];

  listen_socket = PR_NewTCPSocket ();
  if (NULL == listen_socket)
    {
      printf ("SERVERLAUNCHER: Failed to create server socket.\n");
      return;
    }

  local_addr_sa.inet.family = PR_AF_INET;
  local_addr_sa.inet.port = PR_htons (thisserver.listen_port);
  local_addr_sa.inet.ip = PR_htonl (PR_INADDR_ANY);

  printf ("Entered into the server thread on %s port %d.\n", MyIPAddress,
	  thisserver.listen_port);


  if (PR_FAILURE == PR_Bind (listen_socket, &local_addr_sa))
    {
      printf ("SERVERLAUNCHER: Failed to bind to port %d (error %d).\n",
	      thisserver.listen_port, PR_GetError ());
      print_err_msg (PR_GetError ());
      return;
    }

  /*   Listen... */
  if (PR_FAILURE == PR_Listen (listen_socket, 10))
    {
      printf ("SERVERLAUNCHER: Listen at port %d failed (error %d).\n",
	      thisserver.listen_port, PR_GetError ());
      print_err_msg (PR_GetError ());
      return;
    }

  while (1)
    {
      /*  accept new connections */
      new_conn_socket =
	PR_Accept (listen_socket, &remote_addr, PR_INTERVAL_NO_TIMEOUT);

      if (NULL == new_conn_socket)
	continue;

      /*  find out their IP address */

      PR_NetAddrToString (&remote_addr, buf2, 256);
      /* This should be pushed into a logfile 
         ** but we'll print it for now.
       */
      printf ("Connection from %s.\n", buf2);
      PR_NetAddrToString (&remote_addr, buf2, 256);

      /*  Start thread to service request */
      if (NULL ==
	  PR_CreateThread (PR_USER_THREAD, ServiceThread,
			   (void *) new_conn_socket, PR_PRIORITY_NORMAL,
			   PR_LOCAL_THREAD, PR_UNJOINABLE_THREAD, 0))
	{
	  printf ("Failed to create thread.");
	}
    }
}


struct gamenode *
read_socket (PRFileDesc * s, struct gamenode *nodeNew)
     /* This function actually reads all input during the connection,
        ** it places each line into a double linked list for later use.
      */
{
  int ret;
  char buffer[BUFFER_SIZE];
  char *buffer_end = buffer;
  char *tmpString[BUFFER_SIZE];
  int a, mark;
  int p = 0;
  char *token;

  while (1 != 0)
    {
      for (a = 0; a < BUFFER_SIZE; a++)
	buffer[a] = '\0';

      ret =
	PR_Recv (s, buffer_end, sizeof (buffer) - (buffer_end - buffer) - 1,
		 0, PR_INTERVAL_NO_TIMEOUT);
      if (ret <= 0)
	return 0;

      /*  move buffer end and terminate result so far */
      buffer_end += ret;
      *buffer_end = '\0';
      mark = a = 0;
      while (a <= (signed) strlen (buffer))
	{
	  if (buffer[a] == '\r' || buffer[a] == '\n')
	    buffer[a] = '\0';
	  a++;
	}

      if (strstr (buffer, "---") != NULL)
	{
	  nodeNew = NULL;
	  return nodeNew;
	}

      /* fscanf the string */
      nodeNew = (struct gamenode *) PR_MALLOC (sizeof (struct gamenode));
      p =
	sscanf (buffer,
		"%f|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%s|",
		&nodeNew->ver, &nodeNew->age_of_game, &nodeNew->maxplayers,
		&nodeNew->maxports, &nodeNew->maxplanets,
		&nodeNew->players_in_game, &nodeNew->ports_in_game,
		&nodeNew->planets_in_game, &nodeNew->value_of_ports,
		&nodeNew->percent_good_players,
		&nodeNew->percent_planets_with_cid, &nodeNew->corps_in_game,
		&nodeNew->ships_in_game, &nodeNew->fighters_in_game,
		&nodeNew->mines_in_game, &nodeNew->gametype,
		&nodeNew->turns_per_day, &nodeNew->percent_planetary_trade,
		&nodeNew->steal_from_buy_port, &nodeNew->initial_fighters,
		&nodeNew->initial_holds, &nodeNew->initial_credits,
		&nodeNew->clear_busts, &nodeNew->last_bust_clear,
		&nodeNew->multiple_photon_fire, &nodeNew->sectors_in_game,
		&nodeNew->display_stardock, &nodeNew->ship_delay,
		&nodeNew->start_with_planet, &nodeNew->local_display_on,
		&nodeNew->classic_ferrengi, &nodeNew->max_regen_per_visit,
		&nodeNew->rob_steal_delay, &nodeNew->invincible_ferrengal,
		&nodeNew->tournament_mode, tmpString);
      if (p != 0)
	break;
    }
  /* strtok the tmpString to split out host and url */
  fprintf (stderr, "tmpString = %s\n", tmpString);
  token = strtok (tmpString, "|");
  nodeNew->host = PR_MALLOC (strlen (token));
  strcpy (nodeNew->host, token);
  token = strtok (NULL, "|");
  nodeNew->url = PR_MALLOC (strlen (token));
  strcpy (nodeNew->url, token);
  token = strtok (NULL, "|");
  nodeNew->gamename = PR_MALLOC (strlen (token));
  strcpy (nodeNew->gamename, token);
  nodeNew->next = NULL;
  return nodeNew;
}



void
ServiceThread (void *pData)
     /* This functions initates the above readsocket function and
        ** starts the command running if the connection is successful.
      */
{
  char strbuf[1000];
  struct gamenode *bgame;
  struct gamenode *cgame;
  PRFileDesc *s;
  PRFileDesc *f;
  char *strbufptr = &strbuf;
  char *mybuf = "Are you talking to me?\n";
  char *mybuf2 = "OK YOUR FUCKING FILE IS OPEN BITCH!\n";
  s = (PRFileDesc *) pData;


  newgame = read_socket (s, newgame);

  // WORKING // WORKING // WORKING
  if (newgame == NULL)
    {
      if ((f = (PRFileDesc *) fopen ("./data.xml", "r")) != NULL)
	{
	  while ((NULL != fgets (f, strlen (strbuf), strbuf)))
	    {
	      fprintf (stderr, "OH BOYS HERE I AM!! INSIDE WHILE LOOP!\n%s\n",
		       strbuf);
	      if (-1 !=
		  (PR_Send
		   (s, strbufptr, strlen (strbufptr), 0, PR_INTERVAL_MAX)))
		{
		  print_err_msg (PR_GetError ());
		}
	    }
	  fclose ((FILE *) f);
	}
      else
	fprintf (stderr, "file READ didn't work for \"data.xml\"\n");
    }

  if (newgame != NULL && nodeOne == NULL)
    nodeOne = newgame;

  if (newgame != NULL && nodeOne != NULL && nodeOne != newgame)
    {
      bgame = nodeOne;
      cgame = nodeOne->next;
      while (cgame != NULL)
	{
	  bgame = bgame->next;
	  cgame = cgame->next;
	}
      bgame->next = newgame;
      newgame->next = NULL;
    }

  /* Input information into a file / database */
  if (newgame != NULL
      && (f = (PRFileDesc *) fopen ("./data.xml", "w+")) != NULL)
    {
      outputXML (f);
      fclose ((FILE *) f);
    }

  PR_Shutdown (s, PR_SHUTDOWN_BOTH);
}
