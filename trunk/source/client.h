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

#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

char good_ranks[23][50] = { "Civilian", "Private", "Private 1st Class",
  "Lance Corporal", "Corporal", "Sergeant", "Staff Sergeant",
  "Gunner Sergeant", "1st Sergeant", "Sergeant Major",
  "Warrant Officer", "Chief Warrant Officer", "Ensign",
  "Lieutenant J.G.", "Lieutenant", "Lieutenant Commander", "Commander",
  "Captain", "Commodore", "Rear Admiral", "Vice Admiral", "Admiral",
  "Fleet Admiral"
};
char evil_ranks[23][50] = { "Civilian", "Nuisance 3rd Class",
  "Nuisance 2nd Class", "Nuisance 1st Class", "Menace 3rd Class",
  "Menace 2nd Class", "Menace 1st Class", "Smuggler 3rd Class",
  "Smuggler 2nd Class", "Smuggler 1st Class", "Smuggler Savant",
  "Robber", "Terrorist", "Pirate", "Infamous Pirate",
  "Notorious Pirate", "Dread Pirate", "Galactic Scourge",
  "Enemy of the State", "Enemy of the People", "Enemy of Humankind",
  "Heinous Overlord", "Prime Evil"
};

char porttypes[10][25] = { "\x1B[1;36mSpecial", "\x1B[0;32mBB\x1B[0;1;36mS",
  "\x1B[0;32mB\x1B[1;36mS\x1B[0;32mB", "\x1B[0;32mB\x1B[0;1;36mSS",
  "\x1B[1;36mS\x1B[0;32mBB", "\x1B[1;36mS\x1B[0;32mB\x1B[1;36mS",
  "\x1B[1;36mSS\x1B[0;32mB", "\x1B[1;36mSSS", "\x1B[0;32mBBB",
  "\x1B[1;36mSpecial"
};


#define MAXWARPS 5000		//until the server sends this
#define MAX_PLANETS 500		//Ditto

enum prompts
{
  command,
  computer,
  quit,
  pt_port,
  autopilot,
  move,
  debug
};

struct sector
{
  int number;
  int warps[MAX_WARPS_PER_SECTOR];
  char *beacontext;
  char *nebulae;
  struct player *players;
  struct port *ports;
  struct planet *planets;
  struct ship *ships;
};

struct player
{
  int number;
  char *name;
  int shipnumb;
  unsigned long exper;
  long align;
  int turns;
  int credits;
  int rank;
  char *title;
  struct player *next;
  struct ship *pship;
  int blownup;

};

struct ship
{
  int number;
  char *name;
  char *type;
  int fighters;
  int shields;
  int holds;
  int colonists;
  int equipment;
  int organics;
  int ore;
  int ownedby;
  int location;
  int turnsperwarp;
  struct ship *next;
  int ported;
  int emptyholds;
  int kills;
};

struct port
{
  char *name;
  int type;
  int maxproduct[3];
  int product[3];
  int credits;
};

struct planet
{
  int number;
  char *name;
  char *type;
  struct planet *next;
};

int getintstuff ();
void junkline ();
int getsectorinfo (int sockid, struct sector *cursector);
int printsector (struct sector *cursector);
int movesector (char *holder, int sockid, int current,
		struct sector *cursector);
int dologin (int sockid);
char *prompttype (enum prompts type, int sector_or_porttype, int sockid);
int getyes (char *answer);
void psinfo (int sockid, int pnumb, struct player *p);
void clearplayer (struct player *curplayer);
void getmyinfo (int sockid, struct player *curplayer);
void printmyinfo (struct player *curplayer);
void printhelp ();
void printwelcome ();
void do_ship_upgrade(int sockid, struct player *curplayer);
void doporting (int sockid, struct player *curplayer);
void debugmode (int sockid);
void getmessages (char *buffer);
void fedcommlink (int sockid);
void whosplaying (int sockid);
//these are added for invisible passwords
int init_nowait_io (void);
int kbhit (void);
int readch (void);
int done_nowait_io (int status);
char *get_invis_password (void);

#endif
