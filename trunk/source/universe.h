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

#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "common.h"

struct port
{ 
  int number;
  char *name;   //I don't know what else to put in here
  int location;
  int maxproduct[3]; //0 for ore, 1 organics, 2 for equipment
  int product[3];    //Same as above
  int credits;
  int type;
  unsigned short invisible;
};

// The type of port is likey so.
// 1 BBS
// 2 BSB
// 3 BSS
// 4 SBB
// 5 SBS
// 6 SSB
// 7 SSS
// 8 BBB
// 9 Class 9 Stardock and BBB
// 0 Class 0 Upgrade and BBB
// B = Buy
// S = Sell


//It is in this order in players.data
struct player
{
  int number;
  char *name;  
  char *passwd;
  int sector;      //If the player is in a ship this is 0
  int ship;        //And this specifies what ship number he's in
  int experience;
  int alignment;
  int turns;
  int credits;
  int lastprice;   //Last price offered by a port.
  int firstprice;  //first price offered by a port
  unsigned short intransit;  //Is the player moving?
  long beginmove;					//At what time the player began moving
  struct realtimemessage *messages; //Holds realtime messages
  int movingto;				 //What sector the player is moving to
  unsigned short loggedin;  //This is not in the file
  unsigned short ported;    //Neither is this
};

struct realtimemessage
{
	struct realtimemessage *nextmessage;
	char *message;
};

//It is in this order in universe.data
struct ship
{
  int number;
  char *name;
  int type;           //Index + 1 of shipinfo array
  int location;        
  int fighters;
  int shields;
  int holds;
  int colonists;
  int equipment;
  int organics;
  int ore;           //poopy, can't remember then name
  int owner;
};

enum planettype{
	M
};

struct planet
{
  int num;
  char *name;
  enum planettype type;
  char *owner;
  char ownertype;
  char *corp;
  struct citadel *ctd;
};

struct citadel
{
  int level;
};

enum listtype
{
  player,
  planet,
  port,
  ship
};

struct list
{
  void *item;
  enum listtype type;
  struct list *listptr;
};

//It is in this order in universe.data
struct sector
{	
  int number;                   //Stores the sector number
  struct sector *sectorptr[MAX_WARPS_PER_SECTOR];  //sector pointers to other sectors
  char *beacontext;             //Test of a Beacon, NULL, if there is no beacon
  char *nebulae;             //I guess this stores the name, I dont know
  struct list *playerlist[2];  //The list of players in the sector
  struct port *portptr;          //Pointer to ports in the sector
  struct list *planets;      //Pointer to list of planets in sector
};


int init_universe(char *filename, struct sector ***array);
int verify_universe(struct sector **array, int sectorcount);
int verify_sector_links(struct sector *test);
void init_playerinfo(char *filename);
void init_shipinfo(char *filename);
void init_portinfo(char *filename);
#endif
