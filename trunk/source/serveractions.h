#ifndef SERVER_ACTIONS_H
#define SERVER_ACTIONS_H

#include "msgqueue.h"

void processcommand (char *buffer, struct msgcommand *data);
void builddescription (int sector, char *buffer, int playernum);
void findautoroute (int from, int to, char *buffer);
void buildplayerinfo (int playernum, char *buffer);
void buildnewplanet (struct player *curplayer, char *planetname, int sector);
void buildshipinfo (int shipnum, char *buffer);
void buildtotalinfo (int pnumb, char *buffer, struct msgcommand *data);
void trading (struct player *curplayer, struct port *curport, char *buffer,
	      struct ship *curship);
void buildnewplayer (struct player *curplayer, char *shipname);
int move_player (struct player *p, struct msgcommand *data, char *buffer);
void buildportinfo (int portnumb, char *buffer);
int intransit (struct msgcommand *data);
void fedcommlink (int playernum, char *message);
void whosonline (char *buffer);
void sendtoallonline (char *message);
void addmessage (struct player *curplayer, char *message);
void sendtosector (int sector, int playernum, int direction);
void saveplayer (int pnumb, char *filename);
void saveship (int snumb, char *filename);
void saveallports ();

#endif