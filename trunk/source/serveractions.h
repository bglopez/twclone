#ifndef SERVER_ACTIONS_H
#define SERVER_ACTIONS_H

#include "msgqueue.h"

void processcommand (char *buffer, struct msgcommand *data);
void builddescription (int sector, char *buffer, int playernum);
void findautoroute (int from, int to, char *buffer);
void planetupgrade(char *buffer, struct planet *curplanet);
void planettake(char *buffer, struct Player *curplayer);
void planetleave(char *buffer, struct Player *curplayer);
void totalplanetinfo(int pnumb, char *buffer); 
void buildplayerinfo (int playernum, char *buffer);
void buildnewplanet (struct Player *curplayer, char *buffer, int sector);
void buildshipinfo (int shipnum, char *buffer);
void buildallshipinfo(char *buffer);
void buildtotalinfo (int pnumb, char *buffer, struct msgcommand *data);
void listplayerships(char *buffer, struct Player *curplayer);
void buildgameinfo(char *buffer);
void do_ship_upgrade(struct Player *curplayer, char *buffer, struct ship *curship); 
void trading (struct Player *curplayer, struct Port *curport, char *buffer,
	      struct ship *curship);
void buildnewplayer (struct Player *curplayer, char *shipname);
int move_player (struct Player *p, struct msgcommand *data, char *buffer);
void buildportinfo (int portnumb, char *buffer);
int intransit (struct msgcommand *data);
void fedcommlink (int playernum, char *message);
void bank_deposit(char *buffer, struct Player *curplayer);
void bank_balance(char *buffer, struct Player *curplayer);
void bank_withdrawl(char *buffer, struct Player *curplayer);
void sellship(char *buffer, struct Player *curplayer, int port_location);
void priceship(char *buffer, int shipnum);
void listships(char *buffer);
void buyship(char *buffer, struct Player *curplayer);
void listhardware(char *buffer, struct Player *curplayer);
void buyhardware(char *buffer, struct Player *curplayer);
void whosonline (char *buffer);
void sendtoallonline (char *message);
void addmessage (struct Player *curplayer, char *message);
void sendtosector (int sector, int playernum, int direction, int planetnum);
void saveplayer (int pnumb, char *filename);
void saveship (int snumb, char *filename);
void saveallports (char *filename);
int innode(int sector);
void listnodes(char *buffer, struct Port *curport);
void nodetravel(char *buffer, struct Player *curplayer);
void parse_attack(char *buffer, struct Player *curplayer);
void attack(struct Player *from, struct Player *to, int num_figs, char *buffer);

#endif
