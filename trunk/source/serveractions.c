#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "shipinfo.h"
#include "parse.h"
#include "hashtable.h"
#include "msgqueue.h"
#include "universe.h"
#include "serveractions.h"
#include "common.h"
#include "portinfo.h"
#include "boxmuller.c"
#include "config.h"

extern struct sector **sectors;
extern struct list *symbols[HASH_LENGTH];
extern struct player *players[MAX_PLAYERS];
extern struct sp_shipinfo shiptypes[SHIP_TYPE_COUNT];
extern struct ship *ships[MAX_SHIPS];
extern struct port *ports[MAX_PORTS];
extern struct config *configdata;

extern int sectorcount;
struct timeval begin,end;

void processcommand(char *buffer, struct msgcommand *data)
{
  struct player *curplayer;
  struct port *curport;
  struct realtimemessage *curmessage;
  float fsectorcount = (float)sectorcount;  //For rand() stuff in newplayer
  int linknum = 0;
  int seconds = 0;

  switch(data->command)
    {
    case ct_describe:
      //fprintf(stderr, "processcommand: Got a describe command\n");
      if ((curplayer = (struct player *)find(data->name, player, symbols, HASH_LENGTH)) == NULL)
	{
	  strcpy(buffer, "BAD");
	  return;
	}
		if (intransit(data))
		{
				  strcpy(buffer, "BAD: Intransit!");
				  return;
		}
      if (curplayer->sector == 0)
	builddescription(ships[curplayer->ship - 1]->location, buffer, curplayer->number);
      else
	builddescription(curplayer->sector, buffer, curplayer->number);

      fprintf(stderr, "The description has been built\n");

      break;
    case ct_move:
      fprintf(stderr, "processcommand: Got a Move command\n");


      //I'm assuming that this will short circuit
      gettimeofday( &begin, 0);
      if (((curplayer = 
	    (struct player *)find(data->name, player, symbols, HASH_LENGTH)) == NULL) ||
	  ((curplayer->sector != 0) ? curplayer->sector : (ships[curplayer->ship - 1]->location) == data->to) 
	  || data->to > sectorcount)   
	{
	  strcpy(buffer, "BAD");
	  return;
	}
		if (intransit(data))
		{
			strcpy(buffer, "BAD: Already moving!");
			return;
		}
      if ((curplayer->turns <= 0) || (curplayer->turns < shiptypes[ships[curplayer->ship - 1]->type - 1].turns))
      {
		//if(move_player(curplayer, data, buffer) < 0) 
		//{
	  		strcpy(buffer, "BAD");
	  		return;
		}
		//}
      while (linknum < MAX_WARPS_PER_SECTOR)
		{
	  		if (sectors[(curplayer->sector == 0) ? ships[curplayer->ship - 1]->location - 1 : 
		     (curplayer->sector - 1)]->sectorptr[linknum] == NULL)
	      	break;
	  		else if (sectors[(curplayer->sector == 0) ? ships[curplayer->ship - 1]->location - 1 : 
		     (curplayer->sector - 1)]->sectorptr[linknum++]->number == data->to)
	    	{
	      	fprintf(stderr, "processcommand: Move was successfull\n");
	      	if (curplayer->sector == 0)
				{
				sendtosector(ships[curplayer->ship -1]->location, curplayer->number, -1);
		  		curplayer = delete(curplayer->name, player, 
			    sectors[ships[curplayer->ship - 1]->location - 1]->playerlist, 1);
		  		ships[curplayer->ship - 1]->location = data->to;
				}
	      	else
				{
				sendtosector(curplayer->sector, curplayer->number, -1);
		  		curplayer = delete(curplayer->name, player, sectors[curplayer->sector - 1]->playerlist, 1);
		  		curplayer->sector = data->to;
				}
	      //Put realtime so and so warps in/out of the sector here.
	      /*gettimeofday( &end, 0);
	      seconds = end.tv_sec - begin.tv_sec;
	      while(seconds != 
		 (shiptypes[ships[curplayer->ship - 1]->type - 1].turns))
	      {
	      gettimeofday(&end, 0);
	      seconds = end.tv_sec - begin.tv_sec;
	      }
	      //Need to put towing into this later*/
	      //curplayer->turns = curplayer->turns - shiptypes[ships[curplayer->ship - 1]->type - 1].turns;
	      //insertitem(curplayer, player, sectors[data->to - 1]->playerlist, 1);
	      //builddescription(data->to, buffer, curplayer->number);
			curplayer->intransit=1;
			curplayer->movingto=data->to;
			curplayer->beginmove = begin.tv_sec;
			strcpy(buffer, "OK: Now moving");
			return;

	    }
	}
      findautoroute((curplayer->sector == 0) ? ships[curplayer->ship - 1]->location : (curplayer->sector), data->to, buffer);
      break;
    case ct_login:
      fprintf(stderr, "processcommand: Got a login command\n");
      if (((curplayer = 
	    (struct player *)find(data->name, player, symbols, HASH_LENGTH)) == NULL) ||
	  (curplayer->loggedin == 1) || (strcmp(curplayer->name, data->name) != 0) ||
	  (strcmp(curplayer->passwd, data->passwd) != 0))
	{
	  strcpy(buffer, "BAD");
	  return;
	}
      curplayer->loggedin = 1;

      if (curplayer->sector == 0)
	builddescription(ships[curplayer->ship - 1]->location, buffer, curplayer->number);
      else
	builddescription(curplayer->sector, buffer, curplayer->number);

      break;
    case ct_newplayer:
      //fprintf(stderr, "processcommand: Got a newplayer command\n");
      if ((curplayer = (struct player *)insert(data->name, player, symbols, HASH_LENGTH)) == NULL)
	{	
	  //fprintf(stderr, "processcommand: player %s already exists\n", data->name);
	  strcpy(buffer, "BAD");
	  return;
	}
      curplayer->passwd = (char *)malloc(strlen(data->passwd) +1);
      curplayer->name = (char *)malloc(strlen(data->name) +1);
      strncpy(curplayer->passwd, data->passwd, strlen(data->passwd)+1);
      strncpy(curplayer->name, data->name, strlen(data->name) + 1);
      curplayer->sector = (int)(fsectorcount*rand()/RAND_MAX +1.0);
      buildnewplayer(curplayer, data->buffer);
      insertitem(curplayer, player, sectors[(curplayer->sector == 0) ? 
		      (ships[curplayer->ship - 1]->location - 1) 
		      : (curplayer->sector - 1)]->playerlist, 1);

      if (curplayer->sector == 0)
	builddescription(ships[curplayer->ship - 1]->location, buffer, curplayer->number);
      else
	builddescription(curplayer->sector, buffer, curplayer->number);
      break;
	 case ct_update:
    	if ((curplayer = (struct player *)find(data->name, player, symbols, HASH_LENGTH)) == NULL)
			{
	  			strcpy(buffer, "BAD");
	  			return;
			}
		if (curplayer->messages != NULL)
			{  //This handles the realtime messages
				fprintf(stderr, "\nprocesscommand: Lookie we have messages!");
				curmessage = curplayer->messages;
				strcpy(buffer, "OK:");
				strcat(buffer, curmessage->message);
				curplayer->messages = curmessage->nextmessage;
				free(curmessage->message);
				free(curmessage);
				return;
			}
		if (intransit(data) == 0)
		{	
			if (curplayer->sector == 0)
				builddescription(ships[curplayer->ship - 1]->location, buffer, curplayer->number);
      	else
				builddescription(curplayer->sector, buffer, curplayer->number);
		}
		else
			strcpy(buffer, "OK: Still in Transit");
		break;
    case ct_playerinfo:
      //fprintf(stderr, "processcommand: Got a playerinfo command\n");
		if (intransit(data))
		{
			strcpy(buffer, "BAD: Moving you can't do that");
			return;
		}
      buildplayerinfo(data->to, buffer);
      break;
    case ct_shipinfo:
      //fprintf(stderr, "processcommand: Got a shipinfo command\n");
		if (intransit(data))
		{
			strcpy(buffer, "BAD: Moving you can't do that");
			return;
		}
      buildshipinfo(data->to, buffer);
      break;
    case ct_logout:
      fprintf(stderr, "processcommand: Got a logout command\n");
      if ((curplayer = (struct player *)find(data->name, player, symbols, HASH_LENGTH)) == NULL)
		{	
	  	//fprintf(stderr, "processcommand: player %s does not exists\n", data->name);
	  	strcpy(buffer, "BAD");
	  return;
		}
		if (intransit(data))
		{
			strcpy(buffer, "BAD: Can't quit while moving!");
			return;
		}
		fprintf(stderr, "\nprocesscommand Player number is '%d', '%d'", curplayer->number,
				curplayer->ship);
		saveplayer(curplayer->number, "./players.data");
		saveship(curplayer->ship, "./ships.data");
      strcpy(buffer, "OK");
      curplayer->loggedin = 0;
      break;
    case ct_portinfo:
      if ((curplayer = (struct player *)find(data->name, player, symbols, HASH_LENGTH)) == NULL)
      {
	strcpy(buffer, "BAD");
	return;
      }
		if (intransit(data))
		{
			strcpy(buffer, "BAD: Moving can't do that!");
			return;
		}
      if (curplayer->sector == 0)
      {
 	 if (sectors[ships[curplayer->ship - 1]->location - 1]->portptr != NULL)
	   buildportinfo(sectors[ships[curplayer->ship - 1]->location - 1]->portptr->number, buffer);
	 else
	 {
	   strcpy(buffer, "BAD");
	   return;
	 }
      }
      else
      {
	 if (sectors[curplayer->sector - 1]->portptr != NULL)
	   buildportinfo(sectors[curplayer->sector - 1]->portptr->number, buffer);
	 else
	 {
	   strcpy(buffer, "BAD");
	   return;
	 }
      }
      break;
    case ct_port:
      //Currently in progress, Order of stuff
      //If no port in sector then BAD!
      //Check if port is in construction or not
      //Remove current player from the sector
      //Do another switch on the input from the player.
      //Which is either TRADE, QUIT, ROB, SMUGGLE, PLANET, LAND, UPGRADE
      //If TRADE or PLANET
      //   Send port info to player
      //   Run trading algorthim
      //      Trading algorthim is based on the cargo of the player or the
      //      cargo of the planet
      //If ROB qualifactions are met allow player to rob credits or 
      //   commodities
      //If SMUGGLE, dunno.. haven't figured that out yet
      //If LAND goto stardock stuff
      //If UPGRADE goto upgrade stuff(aka fighers, shields or holds)
      //If QUIT, duh.. 
      //
      if ((curplayer = (struct player *)find(data->name, player, symbols, HASH_LENGTH)) == NULL)
	{
	  strcpy(buffer, "BAD");
	  return;
	}
		if (intransit(data))
		{
			strcpy(buffer, "BAD: Can't port while moving!");
			return;
		}
      if (curplayer->sector == 0)
      {
         if (sectors[ships[curplayer->ship - 1]->location - 1]->portptr != NULL)
	    curport = sectors[ships[curplayer->ship - 1]->location - 1]->portptr;
	 else
	    curport = NULL;
      }
      else 
      {
         if (sectors[curplayer->sector -1]->portptr != NULL)
            curport = sectors[curplayer->sector -1]->portptr;
	 else
	    curport = NULL;
      }
      if (curport != NULL)
      {
	 strcpy(buffer, data->buffer);
	 /*if (curplayer->ported == 0)
	 {
	    curplayer = delete(curplayer->name, player, 
	        sectors[curport->location - 1]->playerlist, 1);
	    curplayer->ported = 1;
	 }*/
         switch(data->pcommand)
	   {
	   case p_trade:
	     trading(curplayer, curport, buffer, ships[curplayer->ship - 1]);
	     break;
	   case p_land:
	     break;
	   case p_negotiate:
	     break;
	   case p_upgrade:
	     break;
	   case p_rob:
	     break;
	   case p_smuggle:
	     break;
	   case p_attack:
	     break;
	   case p_quit:
             //insertitem(curplayer, player, sectors[curport->location -1]->playerlist, 1);
	     break;
	   default:
	     break;
	   }
      }
      else
      { 
 	strcpy(buffer, "BAD");
	return;
      }
      break;
    case ct_info:
      if ((curplayer = (struct player *)find(data->name, player, symbols, HASH_LENGTH)) == NULL)
	{
	  strcpy(buffer, "BAD");
	  return;
	}
      buildtotalinfo(curplayer->number, buffer, data);
      break;
    default:
      //fprintf(stderr, "processcommand: Got a bogus command\n");
      strcpy(buffer, "BAD");
    }
  return;
}

//This wants the sector number, not array posistion
void builddescription(int sector, char *buffer, int playernum)
{
  int linknum = 1;
  struct list *element;
  int p = 0;

  buffer[0] = '\0';
  addint(buffer, sector, ':', BUFF_SIZE);

  //This is safe b/c no sector has no warps
  while(linknum < MAX_WARPS_PER_SECTOR  && sectors[sector - 1]->sectorptr[linknum] != NULL)
    addint(buffer, sectors[sector - 1]->sectorptr[linknum++ - 1]->number, ',', BUFF_SIZE);
  addint(buffer, sectors[sector - 1]->sectorptr[linknum - 1]->number, ':', BUFF_SIZE);

  addstring(buffer, sectors[sector - 1]->beacontext, ':', BUFF_SIZE);
  if (strlen(sectors[sector-1]->nebulae) == 0 || strlen(sectors[sector-1]->nebulae) == 1)
		addstring(buffer, "", ':', BUFF_SIZE);
  else
		addstring(buffer, sectors[sector - 1]->nebulae, ':', BUFF_SIZE);
  if (sectors[sector - 1]->portptr != NULL)
  {
     if (sectors[sector - 1]->portptr->invisible == 0)
     {
       addstring(buffer, sectors[sector - 1]->portptr->name, ':', BUFF_SIZE);
       addint(buffer, sectors[sector - 1]->portptr->type, ':', BUFF_SIZE);
     }
     else
     {
       addstring(buffer, "", ':', BUFF_SIZE);
       addstring(buffer, "", ':', BUFF_SIZE);
     }
  }
  else
  {
     addstring(buffer, "", ':', BUFF_SIZE);
     addstring(buffer, "", ':', BUFF_SIZE);
  }
  element = sectors[sector - 1]->playerlist[0];
  if (element == NULL)
    addstring(buffer, "", ':', BUFF_SIZE);
  else
    {
      do
	{
	  if (((struct player *)element->item)->number != playernum)
	    {
	      if (p != 0)
		addint(buffer, p, ',', BUFF_SIZE);
	      p = ((struct player *)element->item)->number;
	    }
	  element = element->listptr;
	}
      while(element != NULL);
      if (p != 0)
	addint(buffer, p, ':', BUFF_SIZE);
      else
	addstring(buffer, "", ':', BUFF_SIZE);
    }

  return;
}

int intransit(struct msgcommand *data)
{
	struct player *curplayer;
	if ((curplayer = (struct player *)find(data->name, player, symbols, HASH_LENGTH)) == NULL)
	  return(-1);
	//fprintf(stderr,"\nintransit: Checking transit");
		  
   gettimeofday(&end, 0);
		if (curplayer->intransit == 1)
		{
			if ((end.tv_sec - curplayer->beginmove) >= (shiptypes[ships[curplayer->ship - 1]->type - 1].turns))
			{
				curplayer->intransit = 0;
				curplayer->beginmove = 0;
				curplayer->turns = curplayer->turns - shiptypes[ships[curplayer->ship - 1]->type - 1].turns;
				insertitem(curplayer, player, sectors[curplayer->movingto - 1]->playerlist, 1);
				sendtosector(curplayer->movingto, curplayer->number, 1);
			return(0);
			}
			else
				return(1);
		}
		else if (curplayer->beginmove == 0)
				  return(0);
		return(0);
}

/*
  This is the auto pilot stuff, it is junk, and needs to be rewritten
*/
/* I'm commenting out all of this old junk just in case it's needed again.
   -Eryndil 4/9/2002
	All of it's deleted now since it's bad junk!
*/

void findautoroute(int from, int to, char *buffer)
{
   int *length = (int *)malloc((sectorcount+1)*sizeof(int));
	int *prev = (int *)malloc((sectorcount+1)*sizeof(int));
	unsigned short *marked = (unsigned short *)malloc((sectorcount+1)*sizeof(unsigned short));
	unsigned short *unmarked = (unsigned short *)malloc((sectorcount+1)*sizeof(unsigned short));
	int shortest=0, done=0, i=0, j=0, counter=0;
	int sectorlist[MAX_WARPS_PER_SECTOR];
	int backpath[30] = {0,0,0,0,0,0,0,0,0,0,
			  				  0,0,0,0,0,0,0,0,0,0,
							  0,0,0,0,0,0,0,0,0,0};
	char temp[50];
	
	for(i=0; i<=sectorcount; i++)
	{
		length[i]=65536;
		prev[i]=0;
		marked[i]=0;
		unmarked[i]=1;
	}
	length[from] = 0;
	while(!done)
	{
		shortest=0;
   	//Find sector with shortest hops to it thats unmarked
		for (counter=1; counter<=sectorcount; counter++)
		{
			if ((length[counter]< length[shortest]) && (unmarked[counter]==1))
					  shortest=counter;
		}
		if (shortest==0)
		{
			break;
		}
		//Use that sector to calculate paths
		i = shortest;
		//Make a list of all adjacent sectors;
		for (counter=0;counter<MAX_WARPS_PER_SECTOR; counter++)
		{
			if(sectors[i-1]->sectorptr[counter]!=NULL)
				sectorlist[counter]=sectors[i-1]->sectorptr[counter]->number;
			else
				sectorlist[counter]=0;
		}
		//now using j as the sector under consideration
		for(counter=0;counter<MAX_WARPS_PER_SECTOR; counter++)
		{
			if (sectorlist[counter] == 0)
					  break;
			if (length[sectorlist[counter]] > (length[i]+1))
			{
				length[sectorlist[counter]]= length[i]+1;
				prev[sectorlist[counter]]=i;
			}
		}
		marked[i]=1;
		unmarked[i]=0;
	}
	//Now we have the shortest path. Using Dijkistra's Algorithm!
	//Now to make the list!
	counter=1;
	backpath[0]=prev[to];
	while(prev[backpath[counter-1]]!=from)
	{
	   backpath[counter]=prev[backpath[counter-1]];
		counter++;
	}	
	sprintf(buffer, ":%d", from);
	for(j=counter-1;j>=0;j--)
	{
		sprintf(temp, ",%d", backpath[j]);
		strcat(buffer, temp);
	}
	sprintf(temp, ",%d:", to);
	strcat(buffer, temp);

	free(length);
	free(prev);
	free(marked);
	free(unmarked);
}

/*
  end of the autopilot stuff (but probably not the end of junk ;)
*/

void saveplayer(int pnumb, char *filename)
{
	char *intptr=(char *)malloc(10);
	char *buffer=(char *)malloc(BUFF_SIZE);
	char *stufftosave=(char *)malloc(BUFF_SIZE);
	FILE *playerfile;
	fpos_t *playerplace;
	int loop=0, len=0;
	
	strcpy(buffer, "\0");
	strcpy(intptr, "\0");
	strcpy(stufftosave, "\0");
	
	sprintf(intptr,"%d", pnumb-1);
	sprintf(stufftosave, "%d:", pnumb);
  	addstring(stufftosave, players[pnumb - 1]->name, ':', BUFF_SIZE);
	addstring(stufftosave, players[pnumb - 1]->passwd, ':', BUFF_SIZE);
	addint(stufftosave, players[pnumb-1]->sector, ':', BUFF_SIZE);
  	addint(stufftosave, players[pnumb - 1]->ship, ':', BUFF_SIZE);
  	addint(stufftosave, players[pnumb - 1]->experience, ':', BUFF_SIZE);
  	addint(stufftosave, players[pnumb - 1]->alignment, ':', BUFF_SIZE);
  	addint(stufftosave, players[pnumb - 1]->turns, ':', BUFF_SIZE);
  	addint(stufftosave, players[pnumb - 1]->credits, ':', BUFF_SIZE);
	len = strlen(stufftosave);

	for (loop=1;loop<=99-len;loop++)
		strcat(stufftosave, " ");
	strcat(stufftosave, "\n");

 
	playerfile = fopen(filename, "r+");
	if (playerfile == NULL)
	{
		fprintf(stderr, "\nsaveplayer: No playerfile! Saving to new one!");
		if ((pnumb-1)!=0)
		{
			fprintf(stderr, "\nsaveplayer: Player is not player 1 for new save file!");
			exit(-1);
		}
		playerfile = fopen(filename, "w");
		fprintf(playerfile, "%s", stufftosave);
		fclose(playerfile);
	}
	while(1)
	{
		strcpy(buffer, "\0");
		fgets(buffer, BUFF_SIZE, playerfile);
		if (strlen(buffer) == 0)
				  break;
		if (strncmp(buffer, intptr, strlen(intptr))==0)
				  fgetpos(playerfile, playerplace);
	}
	fsetpos(playerfile, playerplace);
	fprintf(playerfile, "%s", stufftosave);
	fclose(playerfile);
	free(intptr);
	free(buffer);
	free(stufftosave);
}

void saveship(int snumb, char *filename)
{
	char *intptr=(char *)malloc(10);
	char *buffer=(char *)malloc(BUFF_SIZE);
	char *stufftosave=(char *)malloc(BUFF_SIZE);
	FILE *playerfile;
	fpos_t *playerplace;
	int loop=0, len;

	strcpy(buffer, "\0");
	strcpy(intptr, "\0");
	strcpy(stufftosave, "\0");
	
	sprintf(intptr,"%d", snumb - 1);
	sprintf(stufftosave, "%d:", snumb);
  	addstring(stufftosave, ships[snumb - 1]->name, ':', BUFF_SIZE);
	addint(stufftosave, ships[snumb - 1]->type, ':', BUFF_SIZE);
	addint(stufftosave, ships[snumb - 1]->location, ':', BUFF_SIZE);
  	addint(stufftosave, ships[snumb - 1]->fighters, ':', BUFF_SIZE);
 	addint(stufftosave, ships[snumb - 1]->shields, ':', BUFF_SIZE);
  	addint(stufftosave, ships[snumb - 1]->holds, ':', BUFF_SIZE);
  	addint(stufftosave, ships[snumb - 1]->colonists, ':', BUFF_SIZE);
  	addint(stufftosave, ships[snumb - 1]->equipment, ':', BUFF_SIZE);
  	addint(stufftosave, ships[snumb - 1]->organics, ':', BUFF_SIZE);
  	addint(stufftosave, ships[snumb - 1]->ore, ':', BUFF_SIZE);
  	addint(stufftosave, ships[snumb - 1]->owner, ':', BUFF_SIZE);
	len = strlen(stufftosave);
	for (loop=1;loop<=99-len;loop++)   //This puts a buffer of space in the save
		strcat(stufftosave, " ");		//file so things don't get overwritten
	strcat(stufftosave, "\n");			//when saving.

	playerfile = fopen(filename, "r+");
	if (playerfile == NULL)
	{
		fprintf(stderr, "\nsaveship: No ship file! Saving to new one!");
		playerfile = fopen(filename, "w");
		fprintf(playerfile, "%s", stufftosave);
		fclose(playerfile);
	}

	while(1)
	{
		strcpy(buffer, "\0");
		fgets(buffer, BUFF_SIZE, playerfile);
		if (strlen(buffer) == 0)
				  break;
		if (strncmp(buffer, intptr, strlen(intptr))==0)
				  fgetpos(playerfile, playerplace);
	}
	fsetpos(playerfile, playerplace);
	fprintf(playerfile, "%s",stufftosave);
	fclose(playerfile);
	free(intptr);
	free(buffer);
	free(stufftosave);

}

void buildplayerinfo(int playernum, char *buffer)
{
  buffer[0] = '\0';
  if (players[playernum - 1] == NULL)
  {
     strcpy(buffer, "BAD");
     return;
  }
  addstring(buffer, players[playernum - 1]->name, ':', BUFF_SIZE);
  addint(buffer, players[playernum - 1]->experience, ':', BUFF_SIZE);
  addint(buffer, players[playernum - 1]->alignment, ':', BUFF_SIZE);
  addint(buffer, players[playernum - 1]->ship, ':', BUFF_SIZE);

  return;
}

void buildshipinfo(int shipnum, char *buffer)
{
  buffer[0] = '\0';
  if (ships[shipnum - 1] == NULL)
  {
     strcpy(buffer, "BAD");
     return;
  }
  addint(buffer, ships[shipnum - 1]->owner, ':', BUFF_SIZE);
  addstring(buffer, ships[shipnum - 1]->name, ':', BUFF_SIZE);
  addstring(buffer, shiptypes[ships[shipnum - 1]->type - 1].name, ':', BUFF_SIZE);
  addint(buffer, ships[shipnum - 1]->fighters, ':', BUFF_SIZE);
  addint(buffer, ships[shipnum - 1]->shields, ':', BUFF_SIZE);

}

void buildtotalinfo(int pnumb, char *buffer, struct msgcommand *data)
{
		  
  buffer[0] = '\0';
  
  addint(buffer, players[pnumb - 1]->number, ':', BUFF_SIZE);
  addstring(buffer, players[pnumb - 1]->name, ':', BUFF_SIZE);
  addint(buffer, players[pnumb - 1]->ship, ':', BUFF_SIZE);
  addint(buffer, players[pnumb - 1]->experience, ':', BUFF_SIZE);
  addint(buffer, players[pnumb - 1]->alignment, ':', BUFF_SIZE);
  addint(buffer, players[pnumb - 1]->turns, ':', BUFF_SIZE);
  addint(buffer, players[pnumb - 1]->credits, ':', BUFF_SIZE);
  addint(buffer, ships[players[pnumb - 1]->ship - 1]->number, ':', BUFF_SIZE);
  addstring(buffer, ships[players[pnumb - 1]->ship - 1]->name, ':', BUFF_SIZE);
  addstring(buffer, shiptypes[ships[players[pnumb - 1]->ship - 1]->type - 1].name, ':', BUFF_SIZE);
  addint(buffer, ships[players[pnumb - 1]->ship - 1]->fighters, ':', BUFF_SIZE);
  addint(buffer, ships[players[pnumb - 1]->ship - 1]->shields, ':', BUFF_SIZE);
  addint(buffer, ships[players[pnumb - 1]->ship - 1]->holds, ':', BUFF_SIZE);
  addint(buffer, ships[players[pnumb - 1]->ship - 1]->colonists, ':', BUFF_SIZE);
  addint(buffer, ships[players[pnumb - 1]->ship - 1]->equipment, ':', BUFF_SIZE);
  addint(buffer, ships[players[pnumb - 1]->ship - 1]->organics, ':', BUFF_SIZE);
  addint(buffer, ships[players[pnumb - 1]->ship - 1]->ore, ':', BUFF_SIZE);
  addint(buffer, ships[players[pnumb - 1]->ship - 1]->owner, ':', BUFF_SIZE);
  if (intransit(data))
		addint(buffer, 0, ':', BUFF_SIZE);
  else
  		addint(buffer, ships[players[pnumb - 1]->ship - 1]->location, ':', BUFF_SIZE);
  addint(buffer, shiptypes[ships[players[pnumb - 1]->ship - 1]->type - 1].turns, ':', BUFF_SIZE);


}

void buildportinfo(int portnumb, char *buffer)
{
 buffer[0] = '\0';
 addint(buffer, ports[portnumb - 1]->number, ':', BUFF_SIZE);
 addstring(buffer, ports[portnumb - 1]->name, ':', BUFF_SIZE);
 addint(buffer, ports[portnumb - 1]->maxproduct[0], ':', BUFF_SIZE);
 addint(buffer, ports[portnumb - 1]->maxproduct[1], ':', BUFF_SIZE);
 addint(buffer, ports[portnumb - 1]->maxproduct[2], ':', BUFF_SIZE);
 addint(buffer, ports[portnumb - 1]->product[0], ':', BUFF_SIZE);
 addint(buffer, ports[portnumb - 1]->product[1], ':', BUFF_SIZE);
 addint(buffer, ports[portnumb - 1]->product[2], ':', BUFF_SIZE);
 addint(buffer, ports[portnumb - 1]->credits, ':', BUFF_SIZE);
 addint(buffer, ports[portnumb - 1]->type, ':', BUFF_SIZE);
}
	
void trading(struct player *curplayer, struct port *curport, char *buffer, struct ship *curship)
{
/*
 * If port is selling we want the first price offered to be
 * offered = (int)sell_base[product]*exp(2)*exp(-maxtype/3000)*exp(-current/maxtype)
 * If port is buying we want the first price offered to be
 * offered = (int)buy_base[product]*exp(maxtype/3000)*exp(-current/maxtype)
 *
 * For those who don't know. the exp(2) is from exp(1)*exp(1) which comes
 * from normalizing the two exponentials in the selling..
 * In the buying the normalization of the exponentials is exp(-1) * exp(1)
 * which is 1.
 *
 * Using the Box-Muller Polar Method for Standard Normal Variables
 * The function box_muller() was obtained from
 * http://www.taygeta.com/pub/c/boxmuller.c
 * on 3/10/2001 
 * because log() has problems evaluating numbers close to zero.
 * 
 * 
 */
 int offered=0;
 int playerprice=0;
 int product=-1;
 int type=0;			//For making life easier
 int holds=0;
 int accepted=0;
 int xpgained=0;
 float mean=0;
 float deviation=0;
 double maxproduct;		//Since 2880/3000 = 0 instead of .96
 double curproduct;		//Since 2880/2880 = 0 instead of .96
 float firstprice;
 float lastprice;
 product = popint(buffer, ":");
 holds = popint(buffer, ":");
 playerprice = popint(buffer, ":");
 maxproduct = curport->maxproduct[product];
 curproduct = curport->product[product];
 firstprice = curplayer->firstprice;
 lastprice = curplayer->lastprice;
 
 if (curplayer->lastprice == 0)
 {
   //0 for Ore, 1 for organics, 2 for equipment, 3 for credits
   if (product != 3)
   {  
      if (portconversion[curport->type][product] == 'B')
      {
	  mean = holds*buy_base_prices[product]*exp(maxproduct/3000)*exp(
	-(1 - curproduct/maxproduct));
	  if ((curproduct + holds) > maxproduct)
	  {
	     strcpy(buffer, "BAD: Port cannot buy more");   //To keep from going out of bounds
	     return;
	  }
      }
      else if(portconversion[curport->type][product] == 'S')
      {
	 mean = holds*sell_base_prices[product]*exp(2)*exp(
	-maxproduct/3000)*exp(-curproduct/maxproduct);
	 if ((curproduct - holds) < 0)
	 {
	    strcpy(buffer, "BAD: Port cannot sell more");  //To keep from going out of bounds
	    return;
	 }
 	 if (holds > (curship->holds - (curship->ore + curship->organics + curship->equipment + curship->colonists)))
 	 {
    	    strcpy(buffer, "BAD: User does not have enough holds");
    	    return;
	 }
      }
      else
	strcpy(buffer, "BAD: Port does not sell or buy");
      deviation = .1*mean;
      offered = box_muller(mean, deviation);
      if (playerprice == -1)  //In case we're getting a test price
      {
	 fprintf(stderr, "Got a test price for %d\n", offered);
	 curplayer->lastprice = 0;
	 curplayer->firstprice = 0;
      }
      else
      {
         curplayer->lastprice=offered;
         curplayer->firstprice=offered;
      }
      xpgained = 0;
      accepted = 0;
   }
 }
 else if (curplayer->lastprice != 0)
 {
   if (product != 3)
   {
      if (portconversion[curport->type][product] == 'B')
      {
	if ((playerprice <= (firstprice/0.967 - 2)) ||
	    (playerprice <= curplayer->firstprice))
	{
	  accepted = 1;
	  xpgained = 0;
	}
	else if ((playerprice >= firstprice/0.967 - 1) &&
		(playerprice <= firstprice/0.967 + 1))
	{
	  accepted = 1;
	  xpgained = 5;
	}
	else if ((playerprice >= firstprice/0.967 + 2) &&
		(playerprice <= firstprice/0.967 + 5))
	{
	  accepted = 1;
	  xpgained = 2;
	}
	else if (playerprice >= 1.1*firstprice/0.967)
	{
	  accepted = 0;
	  xpgained = 0;
	  offered = curplayer->lastprice;
	}
	else if ((playerprice > 1.05*firstprice/0.967) &&
		(playerprice < 1.1*firstprice/0.967))
	{
	  accepted = 0;
	  xpgained = 0;
	  offered = curplayer->lastprice + 1;
	}
	else if (playerprice <= 1.05*firstprice/0.967)
	{
	  offered = (firstprice/0.967 + lastprice)/2;
	  accepted = 0;
	  xpgained = 0;
	}
	if (offered >= playerprice)
	{
	  accepted = 1;
	}
	holds = 0 - holds;     //If buying from player want to decriment holds
      }
      else if (portconversion[curport->type][product] == 'S')
      {
        fprintf(stderr, "Offered price is %d, They have %d\n", playerprice,
	      curplayer->credits);
        if (playerprice > curplayer->credits)  //In case someones trying to
        {					     //out fox the system
	   buffer[0] = '\0';
	   addint(buffer, curplayer->lastprice, ':', BUFF_SIZE);
	   addint(buffer, accepted, ':', BUFF_SIZE);
	   addint(buffer, xpgained, ':', BUFF_SIZE);
	   return;
        }
	if (holds > (curship->holds - (curship->ore + curship->organics + curship->equipment + curship->colonists)))
	{
    	   strcpy(buffer, "BAD: User does not have enough holds");
    	   return;
	}
	if ((playerprice >= firstprice*0.967 + 2) ||
	    (playerprice >= curplayer->firstprice))
	{
	   accepted = 1;
	   xpgained = 0;
	}
	else if ((playerprice <= firstprice*0.967 + 1) &&
		(playerprice >= firstprice*0.967 - 1))
	{
	   accepted = 1;
	   xpgained = 5;
	}
	else if ((playerprice <= firstprice*0.967 - 2) &&
		(playerprice >= firstprice*0.967 - 5))
	{
	   accepted = 1;
	   xpgained = 2;
	}
	else if (playerprice <= .9*firstprice*0.967)
	{
	   accepted = 0;
	   xpgained = 0;
	}
	else if ((playerprice > .9*firstprice*0.967) &&
		(playerprice <= .95*firstprice*0.967))
	{
	   accepted = 0;
	   xpgained = 0;
	   offered = curplayer->lastprice - 1;
	}
	else if (playerprice > .95*firstprice*0.967)
	{
	   accepted = 0;
	   xpgained = 0;
	   offered = (lastprice + firstprice*0.967)/2;
	}
	if (offered <= playerprice)
	{
	   accepted = 1;
	}
	playerprice = 0 - playerprice;  //Deduction from players credits
      }
   }
 }
 if (accepted == 1)
 {
   fprintf(stderr, "Price accepted!\n");
   curplayer->lastprice = curplayer->firstprice = 0;
   curplayer->experience = curplayer->experience + xpgained;
   curplayer->credits = curplayer->credits + playerprice;
   switch(product)
   {
     case 0:
       curship->ore = curship->ore + holds;
       type = 2;
       break;
     case 1:
       curship->organics = curship->organics + holds;
       type = 1;
       break;
     case 2:
       curship->equipment = curship->equipment + holds;
       type = 0;
       break;
     default:
       break;
   }
   curport->product[product] = curport->product[product] - holds;
   curport->credits = curport->credits - playerprice;
   if (curport->credits < 0)
     curport->credits = 0; 
   if (portconversion[curport->type][product] == 'B')
   { 
      if (curproduct/maxproduct >= .9)	//If past .9 full of buying
      {   
	 if ((curport->type != 0) || (curport->type != 9))
	 {
	   if (curport->type == 8)
	      curport->type = (int)pow(2,type);
	   else
	      curport->type =+ (int)pow(2,type); //Switch to selling
	 }
      }

   }
   else if (portconversion[curport->type][product] == 'S')
   {
     if (curproduct/maxproduct <= .1) //If past %10 selling 
     {
	 if ((curport->type != 0) || (curport->type != 9))
	 {
	   if ((curport->type - (int)pow(2,type)) == 0)
	      curport->type = 8;      //Switch to buying
	   else
	      curport->type =- (int)pow(2,type);
	 }
     }
   }
 }
 else if (playerprice != -1)
   curplayer->lastprice = offered;
 buffer[0] = '\0';
 addint(buffer, offered, ':', BUFF_SIZE);
 addint(buffer, accepted, ':', BUFF_SIZE);
 addint(buffer, xpgained, ':', BUFF_SIZE);
}
void buildnewplayer(struct player *curplayer, char *shipname)
{

  int i;    //A counter
  struct ship *curship;
  for (i=0; i<=MAX_PLAYERS; i++)
  {
    if (players[i] == NULL)
	break;
  }
  curplayer->number = i + 1;
  players[i] = curplayer;

  for (i=0; i<=MAX_SHIPS; i++)
  {
    if (ships[i] == NULL)
	 break;
  }
  curplayer->experience = 0;
  curplayer->alignment = 0;
  curplayer->turns = configdata->turnsperday; 
  curplayer->credits = configdata->startingcredits;
  curplayer->lastprice = 0;
  curplayer->firstprice = 0;
  curplayer->ported = 0;
  curplayer->loggedin = 1;
  if ((curship = (struct ship *)insert(shipname, ship, symbols, HASH_LENGTH)) == NULL)
  {
     fprintf(stderr, "buildnewplayer: duplicate shipname");
     exit(-1);
  }
  curship->number = i + 1;
  curship->name = (char *)malloc(strlen(shipname) + 1);
  strcpy(curship->name, shipname);
  curship->location = curplayer->sector;
  curship->type = 1;           //Start in a Merchant Cruiser
  curship->fighters = configdata->startingfighters;
  curship->shields = 0;
  curship->holds = configdata->startingholds;
  curship->colonists = 0;
  curship->equipment = 0;
  curship->organics = 0;
  curship->ore = 0;
  curship->owner = curplayer->number;
  curplayer->ship = curship->number;
  curplayer->sector = 0;               //The player is now in a ship
  ships[i] = curship;
}

int move_player(struct player *p, struct msgcommand *data, char *buffer)
{
	int linknum=0;
      
	fprintf(stderr, "processcommand: Got a Move command\n");

      //I'm assuming that this will short circuit
      if (((p = (struct player *)find(data->name, player, symbols, HASH_LENGTH)) == NULL) || ((p->sector != 0) ? p->sector : (ships[p->ship - 1]->location) == data->to) || data->to > sectorcount)  
			return -1; 
      if ((p->turns <= 0) || (p->turns < shiptypes[ships[p->ship - 1]->type - 1].turns)) 
			return -1;

      while (linknum < MAX_WARPS_PER_SECTOR)
		{
	  		if (sectors[(p->sector == 0) ? ships[p->ship - 1]->location - 1 : (p->sector - 1)]->sectorptr[linknum] == NULL)
	      	break;
	  		else if (sectors[(p->sector == 0) ? ships[p->ship - 1]->location - 1 : (p->sector - 1)]->sectorptr[linknum++]->number == data->to)
	  		{
	      	fprintf(stderr, "processcommand: Move was successfull\n");
	      	if (p->sector == 0)
				{
		  			p = delete(p->name, player, sectors[ships[p->ship - 1]->location - 1]->playerlist, 1);
		  			ships[p->ship - 1]->location = data->to;
				}
	      	else
				{
		  			p = delete(p->name, player, sectors[p->sector - 1]->playerlist, 1);
		  			p->sector = data->to;
				}
	      	//Put realtime so and so warps in/out of the sector here.
	      	//Need to put towing into this later
	      	p->turns = p->turns - shiptypes[ships[p->ship - 1]->type - 1].turns;
	      	insertitem(p, player, sectors[data->to - 1]->playerlist, 1);
	      	builddescription(data->to, buffer, p->number);

	      	return data->to;
	    		}
			}
      	findautoroute((p->sector == 0) ? ships[p->ship - 1]->location : (p->sector), data->to, buffer);
	return data->to;
}

void addmessage(struct player *curplayer, char *message)
{
   struct realtimemessage *curmessage=NULL, *newmessage=NULL;
	
	curmessage=curplayer->messages;
	newmessage = (struct realtimemessage *)malloc(sizeof(struct realtimemessage));
	if (curmessage != NULL)
	{
   while(curmessage->nextmessage != NULL)
		  curmessage=curmessage->nextmessage;
	}
	newmessage->message = (char *)malloc(BUFF_SIZE);
	newmessage->nextmessage = NULL;
	strcpy(newmessage->message, message);
	//fprintf(stderr, "\naddmessage: Adding message %s", message);
	if (curplayer->messages == NULL)
	{
		curplayer->messages=newmessage;
		//fprintf(stderr,"\naddmessage: Look '%s's basemessage is NULL", curplayer->name);
	}
	else
		curmessage->nextmessage = newmessage;
	
}

void sendtosector(int sector, int playernum, int direction)
{
   struct list *element;
	char buffer[50];
	char temp[5];
	struct realtimemessage *curmessage=NULL;
	int p=0;	
  
   sprintf(temp, ":%d:", direction);
	//For direction 1 is <name> warps into, -1 is <name> warps out of
 	element = sectors[sector - 1]->playerlist[0];
	strcpy(buffer, players[playernum -1]->name);
	strcat(buffer, temp);
  	if (element == NULL)
	{
   	return;
	}
  	else
	{
      do
		{
	  		if (((struct player *)element->item)->number != playernum)
	    	{
	      	if ((p != 0) && (players[p-1]->loggedin))
					addmessage(players[p-1], buffer);
	      	p = ((struct player *)element->item)->number;
	    	}
	  		element = element->listptr;
		}
      while(element != NULL);
      if ((p != 0) && (players[p-1]->loggedin))
		  addmessage(players[p-1], buffer);
      else
			;
	}


}
