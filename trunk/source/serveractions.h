#ifndef SERVER_ACTIONS_H
#define SERVER_ACTIONS_H

#include "msgqueue.h"

void processcommand(char *buffer, struct msgcommand *data);
void builddescription(int sector, char *buffer, int playernum);
void findautoroute(int from, int to, char *buffer);
int checkall(int from, int to, int depth, int *beenthere);
int buildsectorlist(int from, int to, int depth, int *beenthere);
void buildplayerinfo(int playernum, char *buffer);
void buildshipinfo(int shipnum, char *buffer);
void buildtotalinfo(int pnumb, char *buffer);
void trading(struct player *curplayer, struct port *curport, char *buffer, struct ship *curship);
void buildnewplayer(struct player *curplayer, char *shipname);
int move_player(struct player* p, struct msgcommand *data, char *buffer);
void buildportinfo(int portnumb, char *buffer);
#endif
