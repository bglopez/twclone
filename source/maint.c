/*
Copyright (C) 2002 Ryan Glasnapp(rglasnap@nmt.edu)

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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "universe.h"
#include "shipinfo.h"
#include "hashtable.h"
#include "maint.h"
#include "config.h"
#include "msgqueue.h"
#include "common.h"
#include "serveractions.h"

extern struct sector **sectors;
extern struct list *symbols[HASH_LENGTH];
extern struct player *players[MAX_PLAYERS];
extern struct sp_shipinfo shiptypes[SHIP_TYPE_COUNT];
extern struct ship *ships[MAX_SHIPS];
extern struct port *ports[MAX_PORTS];
extern struct config *configdata;
extern time_t starttime;
extern int sectorcount;

time_t *timeptr;

void *
background_maint (void *threadinfo)
{
  time_t curtime;
  struct tm *timenow;
  int lastregen = -1;
  int lastday = -1;
  int loop = 0;

  free (threadinfo);

  while (1)
    {
      curtime = time (timeptr);
      timenow = localtime (&curtime);
      if (lastregen == -1)
	lastregen = timenow->tm_hour;
      if (lastday == -1)
	lastday = timenow->tm_yday;
      if ((curtime - starttime) % configdata->autosave * 60)	//Autosave
	;			//saveall();
      if ((timenow->tm_hour == lastregen + 1)
	  || ((timenow->tm_hour == 0) && (lastregen == 23)))
	{
	  lastregen = timenow->tm_hour;
	  fprintf (stderr, "\nRegen turns by the hour!");
	  loop = 0;
	  while (players[loop] != NULL)
	    {
	      players[loop]->turns =
		players[loop]->turns + configdata->turnsperday / 24;
	      if (players[loop]->turns > configdata->turnsperday)
		players[loop]->turns = configdata->turnsperday;
	      loop++;
	    }
	}
      if ((timenow->tm_yday == lastday + 1)
	  || ((timenow->tm_yday == 0) && (lastday == 365)))
	{
	  lastday = timenow->tm_yday;
	  fprintf (stderr, "\nRegen leftover turns!");
	  while (players[loop] != NULL)
	    {
	      players[loop]->turns =
		players[loop]->turns + configdata->turnsperday % 24;
	      if (players[loop]->turns > configdata->turnsperday)
		players[loop]->turns = configdata->turnsperday;
	      loop++;
	    }
	}
      if ((curtime - starttime) % configdata->processinterval == 0)
	{
	  //Process real time stuff?
	}
      if ((curtime - starttime) % 3 * configdata->processinterval == 0)
	{
	  //Alien movement here.
	}
    }
}
