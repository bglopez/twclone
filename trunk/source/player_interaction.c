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

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "parse.h"
#include "msgqueue.h"
#include "common.h"
#include "player_interaction.h"



/*
  makeplayerthreads

  This thread sits and waits for network connections, when it gets them,
  it spews forth another thread to handle them
 */

void *
makeplayerthreads (void *threadinfo)
{
  int sockid = ((struct connectinfo *) threadinfo)->sockid,
    msgidin = ((struct connectinfo *) threadinfo)->msgidin, sockaid,
    msgidout = ((struct connectinfo *) threadinfo)->msgidout;

  pthread_t threadid;

  free (threadinfo);
  do
    {
      threadinfo =
	(struct connectinfo *) malloc (sizeof (struct connectinfo));
      sockaid = acceptnewconnection (sockid);

      //putting the info in the special struct for the thread
      ((struct connectinfo *) threadinfo)->sockid = sockaid;
      ((struct connectinfo *) threadinfo)->msgidin = msgidin;
      ((struct connectinfo *) threadinfo)->msgidout = msgidout;

      //make the threads, passing them the stuff to connect to the client
      if (pthread_create (&threadid, NULL, handle_player, (void *) threadinfo)
	  != 0)
	{
	  perror ("Unable to Create Thread");
	  exit (-1);
	}

    }
  while (1);			//we want this to last forever

  close (sockid);

  return NULL;
}


/*
  handle_player

  This is the function that the thread runs.  It handles all of the communication
  for the players.
*/

void *
handle_player (void *threadinfo)
{
  int sector, sockid = ((struct connectinfo *) threadinfo)->sockid,
    msgidout = ((struct connectinfo *) threadinfo)->msgidout,
    msgidin = ((struct connectinfo *) threadinfo)->msgidin,
    commandgood, loggedin;
  char inbuffer[BUFF_SIZE], outbuffer[BUFF_SIZE],
    name[MAX_NAME_LENGTH + 1], passwd[MAX_NAME_LENGTH + 1], temp[BUFF_SIZE];

  struct msgcommand data;

  free (threadinfo);

  printf ("Thread %d: Created\n", (int) pthread_self ());
  loggedin = 0;

  do
    {
      commandgood = 0;

      outbuffer[0] = '\0';

      if (recvinfo (sockid, inbuffer) == -1)
	pthread_exit (NULL);
      
      //fprintf(stderr, "handle_player: I got '%s' as the messagem and loggedin = %d\n", 
      //inbuffer, loggedin);

      //parse stuff from client, should be expanded, modularized
      if (strncmp (inbuffer, "DESCRIPTION", 12) == 0 && loggedin)
	{
	  printf ("Thread %d: Player Querried\n", (int) pthread_self ());
	  strcpy (data.name, name);
	  data.command = ct_describe;
	  commandgood = 1;
	}
      else if (((strncmp (inbuffer, "USER", 4) == 0) ||
		(strncmp (inbuffer, "NEW", 3) == 0)) && !loggedin)
	{
	  popstring (inbuffer, temp, " ", BUFF_SIZE);

	  data.command = (strlen (temp) == 4) ? ct_login : ct_newplayer;

	  popstring (inbuffer, name, ":", BUFF_SIZE);
	  popstring (inbuffer, passwd, ":", BUFF_SIZE);
	  popstring (inbuffer, data.buffer, ":", BUFF_SIZE);

	  fprintf (stderr,
		   "Thread %d: Player '%s' trying to login with passwd '%s'\n",
		   (int) pthread_self (), name, passwd);
	  strcpy (data.name, name);
	  strcpy (data.passwd, passwd);

	  commandgood = 2;
	}
      else if (strncmp (inbuffer, "PLAYERINFO", 10) == 0 && loggedin)
	{
	  popstring (inbuffer, temp, " ", BUFF_SIZE);
	  data.to = popint (inbuffer, ":");
	  data.command = ct_playerinfo;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "UPDATE", 6) == 0 && loggedin)
	{
	  strcpy (data.name, name);
	  data.command = ct_update;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "ONLINE", 6) == 0)
	{
	  data.command = ct_online;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "FEDCOMM", 7) == 0 && loggedin)
	{
	  popstring (inbuffer, temp, " ", BUFF_SIZE);
	  popstring (inbuffer, temp, ":", BUFF_SIZE);
	  strcpy (data.buffer, temp);
	  data.command = ct_fedcomm;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "LAND", 4) == 0 && loggedin)
	{
	  popstring (inbuffer, temp, " ", BUFF_SIZE);
	  data.to = popint (inbuffer, ":");
	  data.command = ct_land;
	  commandgood = 1;
	}

      else if (strncmp (inbuffer, "SCAN", 4) == 0 && loggedin)
	{
	  popstring (inbuffer, temp, " ", BUFF_SIZE);
	  data.command = ct_scan;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "BEACON", 6) == 0 && loggedin)
	{
	  data.command = ct_beacon;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "TOW", 3) == 0 && loggedin)
	{
	  data.command = ct_tow;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "LISTNAV", 3) == 0 && loggedin)
	{
	  data.command = ct_listnav;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "TRANSPORTER", 11) == 0 && loggedin)
	{
	  data.command = ct_transporter;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "GENESIS", 7) == 0 && loggedin)
	{
	  popstring(inbuffer, temp, " ", BUFF_SIZE);
	  popstring(inbuffer, data.buffer, ":", BUFF_SIZE);
	  data.command = ct_genesis;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "JETTISON", 8) == 0 && loggedin)
	{
	  data.command = ct_jettison;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "INTERDICT", 9) == 0 && loggedin)
	{
	  data.command = ct_interdict;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "ATTACK", 6) == 0 && loggedin)
	{
	  data.command = ct_attack;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "ETHERPROBE", 10) == 0 && loggedin)
	{
	  data.command = ct_etherprobe;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "FIGHTERS", 8) == 0 && loggedin)
	{
	  data.command = ct_fighters;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "LISTFIGHTERS", 12) == 0 && loggedin)
	{
	  data.command = ct_listfighters;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "MINES", 5) == 0 && loggedin)
	{
	  data.command = ct_mines;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "LISTMINES", 9) == 0 && loggedin)
	{
	  data.command = ct_listmines;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "PORTUPGRADE", 11) == 0 && loggedin)
	{
	  data.command = ct_portconstruction;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "SETNAVS", 7) == 0 && loggedin)
	{
	  data.command = ct_setnavs;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "STATUS", 6) == 0 && loggedin)
	{
	  data.command = ct_gamestatus;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "HAIL", 4) == 0 && loggedin)
	{
	  data.command = ct_hail;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "SUBSPACE", 8) == 0 && loggedin)
	{
	  data.command = ct_subspace;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "LISTWARPS", 9) == 0 && loggedin)
	{
	  data.command = ct_listwarps;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "CIM", 3) == 0 && loggedin)
	{
	  data.command = ct_cim;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "AVOID", 5) == 0 && loggedin)
	{
	  data.command = ct_avoid;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "LISTAVOIDS", 10) == 0 && loggedin)
	{
	  data.command = ct_listavoids;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "SELFDESTRUCT", 12) == 0 && loggedin)
	{
	  data.command = ct_selfdestruct;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "LISTSETTINGS", 12) == 0 && loggedin)
	{
	  data.command = ct_listsettings;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "SETTINGS", 8) == 0 && loggedin)
	{
	  data.command = ct_updatesettings;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "ANNOUNCE", 8) == 0 && loggedin)
	{
	  data.command = ct_dailyannouncement;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "PHOTON", 6) == 0 && loggedin)
	{
	  data.command = ct_firephoton;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "READMAIL", 8) == 0 && loggedin)
	{
	  data.command = ct_readmail;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "SENDMAIL", 8) == 0 && loggedin)
	{
	  data.command = ct_sendmail;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "TIME", 4) == 0 && loggedin)
	{
	  data.command = ct_shiptime;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "DISRUPTOR", 9) == 0 && loggedin)
	{
	  data.command = ct_usedisruptor;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "DAILYLOG", 8) == 0 && loggedin)
	{
	  data.command = ct_dailylog;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "ALIENRANKS", 10) == 0 && loggedin)
	{
	  data.command = ct_alienranks;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "LISTPLAYERS", 11) == 0 && loggedin)
	{
	  data.command = ct_listplayers;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "LISTPLANETS", 11) == 0 && loggedin)
	{
	  data.command = ct_listplanets;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "LISTSHIPS", 9) == 0 && loggedin)
	{
	  data.command = ct_listships;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "LISTCORPS", 9) == 0 && loggedin)
	{
	  data.command = ct_listcorps;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "JOINCORP", 8) == 0 && loggedin)
	{
	  data.command = ct_joincorp;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "MAKECORP", 8) == 0 && loggedin)
	{
	  data.command = ct_makecorp;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "CREDTRANS", 9) == 0 && loggedin)
	{
	  data.command = ct_credittransfer;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "FIGTRANS", 8) == 0 && loggedin)
	{
	  data.command = ct_fightertransfer;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "MINETRANS", 9) == 0 && loggedin)
	{
	  data.command = ct_minetransfer;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "SHIELDTRANS", 11) == 0 && loggedin)
	{
	  data.command = ct_shieldtransfer;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "CORPQUIT", 8) == 0 && loggedin)
	{
	  data.command = ct_quitcorp;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "CORPPLANETS", 11) == 0 && loggedin)
	{
	  data.command = ct_listcorpplanets;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "ASSETS", 6) == 0 && loggedin)
	{
	  data.command = ct_showassets;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "CORPMEMO", 8) == 0 && loggedin)
	{
	  data.command = ct_corpmemo;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "DROPMEMBER", 10) == 0 && loggedin)
	{
	  data.command = ct_dropmember;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "CORPPASS", 8) == 0 && loggedin)
	{
	  data.command = ct_corppassword;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "SHIPINFO", 8) == 0 && loggedin)
	{
	  popstring (inbuffer, temp, " ", BUFF_SIZE);
	  data.to = popint (inbuffer, ":");
	  data.command = ct_shipinfo;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "QUIT", 5) == 0 && loggedin)
	{
	  strcpy (data.name, name);
	  data.command = ct_logout;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "PORTINFO", 8) == 0 && loggedin)
	{			//don't move this below port.. otherwise
	  strcpy (data.name, name);	//badstuff will happen
	  data.command = ct_portinfo;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "PORT ", 5) == 0 && loggedin)
	{
	  printf ("Thread %d: Player attempting to port\n",
		  (int) pthread_self ());
	  strcpy (data.name, name);
	  popstring (inbuffer, temp, " ", BUFF_SIZE);
	  popstring (inbuffer, temp, ":", BUFF_SIZE);
	  if (strncmp (temp, "TRADE", 5) == 0)
	    data.pcommand = p_trade;
	  else if (strncmp (temp, "LAND", 4) == 0)
	    data.pcommand = p_land;
	  else if (strncmp (temp, "NEGOTIATE", 9) == 0)
	    data.pcommand = p_trade;
	  else if (strncmp (temp, "UPGRADE", 7) == 0)
	    data.pcommand = p_upgrade;
	  else if (strncmp (temp, "ROB", 3) == 0)
	    data.pcommand = p_rob;
	  else if (strncmp (temp, "SMUGGLE", 7) == 0)
	    data.pcommand = p_smuggle;
	  else if (strncmp (temp, "ATTACK", 6) == 0)
	    data.pcommand = p_attack;
	  else if (strncmp (temp, "QUIT", 4) == 0)
	    data.pcommand = p_quit;

	  strncpy (data.buffer, inbuffer, 30);
	  data.command = ct_port;
	  commandgood = 1;
	}
      else if (strncmp (inbuffer, "MYINFO", 6) == 0 && loggedin)
	{
	  strcpy (data.name, name);
	  data.command = ct_info;
	  commandgood = 1;
	}
      else if ((sector = strtol (inbuffer, NULL, 10)) != 0 && loggedin)
	{
	  printf ("Thread %d: Player moving to %d\n", (int) pthread_self (),
		  sector);
	  strcpy (data.name, name);
	  data.command = ct_move;
	  data.to = sector;
	  commandgood = 1;
	}

      if (commandgood)
	{
	  senddata (msgidin, &data, pthread_self ());
	  getmsg (msgidout, outbuffer, pthread_self ());
	}
      else
	strcpy (outbuffer, "BAD\n");

      if (!loggedin && strncmp (outbuffer, "BAD\n", 3) != 0
	  && commandgood == 2)
	loggedin = 1;

      if (sendinfo (sockid, outbuffer) == -1)
	pthread_exit (NULL);

    }
  while (strcmp (inbuffer, "QUIT") != 0);

  //close our socket
  close (sockid);

  fprintf (stderr, "Thread %d: Just closed the socket, exiting\n",
	   (int) pthread_self ());

  return NULL;
}
