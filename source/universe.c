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

/* 
   originally by Ryan
   modified Jason Garcowski 
   7/15/00

   universe.c

   Contains all of the functions to init the universe
*/

/*
  Important note, these should be called in the order:
  universe, ship, player
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "shipinfo.h"
#include "parse.h"
#include "hashtable.h"
#include "universe.h"
#include "common.h"
#include "planet.h"

extern struct list *symbols[HASH_LENGTH];
extern struct player *players[MAX_PLAYERS];
extern struct planet *planets[MAX_TOTAL_PLANETS];
extern struct ship *ships[MAX_SHIPS];
extern struct port *ports[MAX_PORTS];
extern struct sector **sectors;

int init_universe(char *filename, struct sector ***array)
{
  int sectorcount = 0, len, pos, sectornum, i, tempsector, sctptrcount;
  FILE *univinfo;
  char buffer[BUFF_SIZE], temp[BUFF_SIZE];

  univinfo = fopen(filename, "r");
  (* array) = NULL;

  do
    {
      buffer[0] = '\0';

      fgets(buffer, BUFF_SIZE, univinfo);
      
      sectornum = popint(buffer, ":");
      
      if (sectornum == 0)
	break;

      if (sectornum > sectorcount)
	{
	  //allocate enough pointers in the array
	  (* array) = (struct sector **) realloc((* array), sectornum * sizeof(struct sector *));
	  
	  //attach the newly allocated sectors to the array
	  for (i = sectorcount; i < sectornum; i++)
	    (* array)[i] = (struct sector *) malloc(sizeof(struct sector));
	  
	  sectorcount = sectornum;
	}
      pos = len;
      
      (* array)[sectornum - 1]->number = sectornum;
      
      sctptrcount = 0;

      popstring(buffer, temp, ":", BUFF_SIZE);
      
      while((tempsector = popint(temp, ",")) != 0 && sctptrcount < MAX_WARPS_PER_SECTOR)
	{
	  //fprintf(stderr, "init_universe: tempsector = %d, sectornum = %d\n", tempsector, sectornum);
	  if (tempsector > sectorcount)
	    {
	      //allocate enough pointers in the array
	      (* array) = 
		(struct sector **) realloc((* array), tempsector * sizeof(struct sector *));
	  
	      //attach the newly allocated sectors to the array
	      for (i = sectorcount; i < tempsector; i++)
		(* array)[i] = (struct sector *) malloc(sizeof(struct sector));
	  
	      sectorcount = tempsector;
	     
	      //I set it to zero now so I can test to make sure it has its own entry
	      (* array)[tempsector - 1]->number = 0;
	    }

	  //make the link from our current sector to where it points.
	  (* array)[sectornum - 1]->sectorptr[sctptrcount++] = (* array)[tempsector - 1];
	}

      //set the last pointer to NULL if applicable
      if (sctptrcount < MAX_WARPS_PER_SECTOR) 
	(* array)[sectornum - 1]->sectorptr[sctptrcount] = NULL;
      //copy the beacon info over
      popstring(buffer, temp, ":", BUFF_SIZE);
      (* array)[sectornum - 1]->beacontext = (char *) malloc(strlen(temp) + 1);
      strncpy((* array)[sectornum - 1]->beacontext, temp, strlen(temp) + 1);
      (* array)[sectornum - 1]->beacontext[strlen(temp)] = '\0';
		if (strlen(temp) == 0)
			strcpy((* array)[sectornum - 1]->beacontext, "\0");

      //copy the nebulae info over
      popstring(buffer, temp, ":", BUFF_SIZE);
      (* array)[sectornum - 1]->nebulae = (char *) malloc(strlen(temp) + 1);
      strncpy((* array)[sectornum - 1]->nebulae, temp, strlen(temp) + 1);
      (* array)[sectornum - 1]->nebulae[strlen(temp)] = '\0';
   	if (strlen(temp) == 0)
			strcpy((* array)[sectornum -1]->nebulae, "\0");
		init_hash_table((* array)[sectornum - 1]->playerlist, 1);
      (* array)[sectornum - 1]->portptr = NULL;
    }
  while (1); 

  fclose(univinfo);
  return sectorcount;
}

void init_playerinfo(char *filename)
{
  FILE *playerinfo;
  char name[MAX_NAME_LENGTH], passwd[MAX_NAME_LENGTH];
  char buffer[BUFF_SIZE];
  int playernum;
  struct player *curplayer;

  for (playernum = 0; playernum < MAX_PLAYERS; playernum++)
    players[playernum] = NULL;

  playerinfo = fopen(filename, "r");
  while (1)
    {
      buffer[0] = '\0';
      fgets(buffer, BUFF_SIZE, playerinfo);
      if (strlen(buffer) == 0)
	break;
      playernum = popint(buffer, ":");
      popstring(buffer, name, ":", MAX_NAME_LENGTH);
      popstring(buffer, passwd, ":", MAX_NAME_LENGTH);

      //fprintf(stderr, "init_playerinfo: popped name '%s' & passwd '%s', buffer = '%s'\n",
      //    name, passwd, buffer);

      if ((curplayer = (struct player *)insert(name, player, symbols, HASH_LENGTH)) == NULL)
	{
	  fprintf(stderr, "init_playerinfo: duplicate player name '%s'\n", name);
	  exit (-1);
	}

      curplayer->sector = popint(buffer, ":");
      curplayer->ship = popint(buffer, ":");
      curplayer->number = playernum;
      curplayer->experience = popint(buffer, ":");
      curplayer->alignment = popint(buffer, ":");
      curplayer->turns = popint(buffer, ":");
      curplayer->credits = popint(buffer, ":");
      curplayer->name = (char *)malloc(strlen(name) + 1);
      curplayer->passwd = (char *)malloc(strlen(passwd) + 1);
      strncpy(curplayer->name, name, strlen(name) + 1);
      strncpy(curplayer->passwd, passwd, strlen(passwd) + 1);
      curplayer->loggedin = 0;
      curplayer->ported = 0;
      curplayer->lastprice = 0;
      curplayer->firstprice = 0;

      if (players[playernum - 1] != NULL)
	{
	  fprintf(stderr, "init_playinfo: duplicate player numbers, exiting...\n");
	  exit(-1);
	}
      players[playernum - 1] = curplayer;

      if (insertitem(curplayer, player,
		     sectors[(curplayer->sector == 0) ? 
			    ships[curplayer->ship - 1]->location - 1 :
			    (curplayer->sector - 1)]->playerlist, 1) == NULL)
	{
	  fprintf(stderr, 
		  "init_playerinfo: unable to add player '%s'to playerlist in sector %d!\n", 
		  name, (curplayer->sector == 0) ? ships[curplayer->ship - 1]->location : 
		  (curplayer->sector));
	  exit(-1);
	}
	

      //Here is where I need to tack this onto the playerlist

      //printf("init_playerinfo: adding '%s' with passwd '%s', in sector '%d'\n",
      //   name, passwd, curplayer->sector);
    }
  
  fclose(playerinfo);
}

void init_shipinfo(char *filename)
{
  FILE *shipfile;
  char buffer[BUFF_SIZE];
  char name[MAX_NAME_LENGTH];
  int x;
  struct ship *curship;

  for (x = 0; x < MAX_SHIPS; x++)
    ships[x] = NULL;

  shipfile = fopen(filename, "r");
  while (1)
    {
      buffer[0] = '\0';
      fgets(buffer, BUFF_SIZE, shipfile);
      if (strlen(buffer) == 0)
	break;
      x = popint(buffer, ":");
      popstring(buffer, name, ":", MAX_NAME_LENGTH);
      if ((curship = (struct ship *)insert(name, ship, symbols, HASH_LENGTH)) == NULL)
	{
	  fprintf(stderr, "init_shipinfo: duplicate shipname '%s'\n", name);
	  exit(-1);
	}
      curship->number = x;
      curship->name = (char *)malloc(strlen(name) + 1);
      strcpy(curship->name, name);
      if ((curship->type = popint(buffer, ":")) > SHIP_TYPE_COUNT)
	{
	  fprintf(stderr, "init_shipinfo: bad ship type number\n");
	  exit(-1);
	}
      curship->location = popint(buffer, ":");
      curship->fighters = popint(buffer, ":");
      curship->shields = popint(buffer, ":");
      curship->holds = popint(buffer, ":");
      curship->colonists = popint(buffer, ":");      
      curship->equipment = popint(buffer, ":");
      curship->organics = popint(buffer, ":");
      curship->ore = popint(buffer, ":");
      curship->owner = popint(buffer, ":");
      if (ships[x - 1] != NULL)
	{
	  fprintf(stderr, "init_shipinfo: duplicate ship numbers, exiting...\n");
	  exit(-1);
	}
      ships[x - 1] = curship;
    }
  fclose(shipfile);

  return;
}

void init_portinfo(char *filename)
{

  FILE *portfile;
  int counter;			//Counter and other general usage
  char buffer[BUFF_SIZE];
  char name[MAX_NAME_LENGTH];
  struct port *curport;

  for (counter=0; counter<=MAX_PORTS; counter++)
      ports[counter]=NULL;
  
  portfile = fopen(filename, "r");
  while(1)
  {
    buffer[0]='\0';
    fgets(buffer, BUFF_SIZE, portfile);
    if (strlen(buffer) == 0)
	    break;
    counter = popint(buffer, ":");
    popstring(buffer, name, ":", MAX_NAME_LENGTH);
    if ((curport = (struct port *)insert(name, port, symbols, HASH_LENGTH)) == NULL)
    {
       fprintf(stderr, "init_portinfo: duplicate portname '%s'\n", name);
       exit(-1);
    }
    curport->number = counter;
    curport->location = popint(buffer, ":");
    curport->maxproduct[0] = popint(buffer, ":"); //MaxOre
    curport->maxproduct[1] = popint(buffer, ":"); //MaxOrganics
    curport->maxproduct[2] = popint(buffer, ":"); //MaxEquipment
    curport->product[0] = popint(buffer, ":");    //Current Ore
    curport->product[1] = popint(buffer, ":");	  //Current Organics
    curport->product[2] = popint(buffer, ":");    //Current Equipment
    curport->credits = popint(buffer, ":");
    curport->type = popint(buffer, ":");
    curport->invisible = popint(buffer, ":");
    curport->name = (char *)malloc(strlen(name) +1);
    strcpy(curport->name, name);

    if (ports[counter -1] != NULL)
    {
       fprintf(stderr, "init_portinfo: Duplicate ship numbers, exiting...\n");
       exit(-1);
    }
    ports[counter - 1] = curport;
    sectors[curport->location - 1]->portptr = curport;
  }
  fclose(portfile);
	    
}

//This stuff isn't used right now.
int verify_universe(struct sector **array, int sectorcount)
{
  int i;

  for (i = 0; i < sectorcount; i++)
    {
      if (array[i] == NULL)
	{
	  printf(" Sector %d does not exist!...", i + 1);
	  return -1;
	}
      else if (verify_sector_links(array[i]) == -1)
	return -1;
    }

  return 0;
}

int verify_sector_links(struct sector *test)
{
  int i, j, good;

  if (test->sectorptr[0] == NULL)
    {
      printf(" Sector %d has no links!...", test->number);
      return -1;
    }
  
  for (i = 0; i < MAX_WARPS_PER_SECTOR; i++)
    {
      if (test->sectorptr[i] == NULL)
	break;
      else 
	{
	  good = 0;
	  for (j = 0; j < MAX_WARPS_PER_SECTOR; j++)
	    {
	      if (test->sectorptr[i]->sectorptr[j] == NULL)
		break;
	      else if (test->sectorptr[i]->sectorptr[j] == test)
		good = 1;
	    }
	  if (good == 0)
	    {
	      printf("Sector %d is linked to Sector %d, but not vice versa!...",
		     test->number, test->sectorptr[i]->number);
	      return -1;
	    }
	}
    }

  return 0;
}
