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

extern struct sector **sectors;
extern struct list *symbols[HASH_LENGTH];
extern struct player *players[MAX_PLAYERS];
extern struct sp_shipinfo shiptypes[SHIP_TYPE_COUNT];
extern struct ship *ships[MAX_SHIPS];
extern struct port *ports[MAX_PORTS];
extern struct config *configdata;

extern int sectorcount;
extern time_t *starttime, *curtime; 

void *background_maint(void *threadinfo)
{
   //int msgidin = ((struct connectinfo *)threadinfo)->msgidin;
	struct msgcommand data;
	char buffer[BUFF_SIZE];
						 
	free(threadinfo);

	while (1)
	{
   	time(curtime);
   	if ((curtime - starttime)% configdata->autosave * 60) //Autosave
     		saveall();
   	if ((curtime - starttime)% 3600 == 0)
      	;//Regen turns; Needs to check system time
   	if ((curtime - starttime)% 86400 == 0)
      	;//Regen fractional turns leftover Needs to check system time
  		if ((curtime - starttime)% configdata->processinterval == 0)
   	{
			//Process real time stuff?
		}
   	if ((curtime - starttime)% 3*configdata->processinterval == 0)
   	{
			//Alien movement here.
		} 
	}
}
