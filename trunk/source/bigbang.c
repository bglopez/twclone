/*
Copyright (C) 2002 Scott Long (link@kansastubacrew.com)

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

//This is the max length of tunnels and dead ends.
#define MAXTUNLEN			6

//Variables that will eventually be inputted by the user when this program
//is initally run.
#define NUMSECTORS			500

//This is the number of ports 
#define NUMPORTS				190

//Percentage of sectors that will have the maximum number of warps in them 
#define MAXJUMPPERCENT		3

//Percentage chance that a final jump will be a one-way
#define ONEWAYJUMPPERCENT	3

//Percentage chance that a tunnel will be a dead end
#define DEADENDPERCENT		30

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "universe.h"

//These are the set-in-stone FedSpace links
//Don't even think about touching these...
const int fedspace[10][6]={ {2,3,4,5,6,7},
							{1,3,7,8,9,10},
							{1,2,4,0,0,0},
							{1,3,5,0,0,0},
							{1,4,6,0,0,0},
							{1,5,7,0,0,0},
							{1,2,6,8,0,0},
							{2,7,0,0,0,0},
							{2,10,0,0,0,0},
							{2,9,0,0,0,0} };
struct sector **sectorlist;
struct config *configdata;
struct port *portlist[NUMPORTS];

int randsectornum[NUMSECTORS-10];

int compsec(const void *cmp1, const void *cmp2);
int randjump(int maxjumplen);
int randomnum(int min, int max);
void secjump(int from, int to);
int freewarp(int sector);
int warpsfull(int sector);
int numwarps(int sector);
void makeports();

int main(void) 
{	int x, y, z, tempint, randint, tosector, fromsector, startsec, secptrcpy, jumpsize;
	int maxjumpsize = (int) (NUMSECTORS * ((double) MAXJUMPPERCENT / 100));
	int usedsecptr = NUMSECTORS - 11;
	int len;
	char *fileline, *tempstr;
	FILE *file;
	struct sector *secptr;

	//Seed our randomizer
	srand((unsigned long) time(NULL));
	
	//Reading config.data file for config data (Duh...)
	printf("\nReading in config.data...");
	init_config("config.data");
	printf("done.\n");
	
	printf("Creating sector array...");
	sectorlist = malloc(NUMSECTORS * sizeof(struct sector *));
	for(x = 0; x < NUMSECTORS; x++)
		sectorlist[x] = malloc(sizeof(struct sector));
	printf("done.\n");
	
	printf("Creating port array...");
	for(x = 0; x<NUMPORTS; x++)
	{
		portlist[x]=NULL;
		sectorlist[x]->portptr=NULL;
	}
	printf("done.\n");

	//Fills in the randsectornum array with numbers 10 to (numsectors - 1)
	for(x=0;x<NUMSECTORS-10;x++)
		randsectornum[x]=x+10;

	printf("Randomly picking sector numbers...");
	//Randomly creates sector numbers to use:
	for(x=NUMSECTORS-11;x>0;x--)
	{	randint = randomnum(0,x);
		tempint = randsectornum[randint];
		randsectornum[randint] = randsectornum[x];
		randsectornum[x] = tempint;
	}
	printf("done.\n");
	
	//Initalize sectorlist with data
	for(x=0;x<NUMSECTORS;x++)
		sectorlist[x]->number = x+1;
	
	printf("Creating Fedspace...");
	//Sets up Fed Space
	for(x=0;x<10;x++)
	{	for(y=0;y<6;y++)
			if(fedspace[x][y]!=0)
				sectorlist[x]->sectorptr[y]=sectorlist[(fedspace[x][y])-1];
		sectorlist[x]->beacontext = "The Federation -- Do Not Dump!";
		sectorlist[x]->nebulae = "The Federation";
	}
	printf("done.\n");
			
	printf("Setting up links from FedSpace out to other sectors...");
	//Sets up 13 jumps from Fed Space to 6jump sectors 
	for(x=0;x<=13;x++)
	{	randint=randomnum(1,5);
		do
		{	fromsector = randomnum(2,9);
		} while(warpsfull(fromsector));
		jumpsize = randjump(3);
	
		for(y=0;y<jumpsize;y++)
		{	tempint = randomnum(maxjumpsize,usedsecptr);
			tosector = randsectornum[tempint];
			randsectornum[tempint] = randsectornum[usedsecptr];
			randsectornum[usedsecptr] = tosector;
			usedsecptr--;
			secjump(fromsector,tosector);
			secjump(tosector,fromsector);
			fromsector = tosector;
		}
		if(randint<4)
		{	secjump(fromsector,x);
			secjump(x,fromsector);
		}
	}
	printf("done.\n");

	printf("Setting up the max warp sectors...");
	//Sets up rest of links for the maxwarp sectors (the meat and potatoes)
	for(x=0;x<maxjumpsize;x++)
	{	for(y=freewarp(x);y<configdata->maxwarps;y++)
		{	randint = randomnum(1,100);
			jumpsize = randjump(MAXTUNLEN);
			startsec = randsectornum[x];
			fromsector = startsec;
			for(z=0;z<jumpsize;z++)
			{	tempint = randomnum(maxjumpsize,usedsecptr);
				tosector = randsectornum[tempint];
				randsectornum[tempint] = randsectornum[usedsecptr];
				randsectornum[usedsecptr] = tosector;
				usedsecptr--;
				
				secjump(fromsector,tosector);
				secjump(tosector,fromsector);
				fromsector = tosector;
			}
			if(randint <= (100 - DEADENDPERCENT))
			{	do
				{	tosector = randsectornum[randomnum(0,maxjumpsize-1)];
				} while(tosector == startsec);
				secjump(fromsector,tosector);
				if(randomnum(1,100) <= (100 - ONEWAYJUMPPERCENT) && !warpsfull(tosector))
					secjump(tosector,fromsector);
			}
		}
	}
	printf("done.\n");
	
	for(x=0;x<maxjumpsize;x++)
	{	tempint = randsectornum[x];
		randsectornum[x] = randsectornum[usedsecptr];
		randsectornum[usedsecptr]=tempint;
		usedsecptr--;
	}

	printf("Using up leftover sector numbers...");	
	while(usedsecptr>=0)  //finishes up creating other sector links...
	{	randint = randomnum(1,100);
		tempint = MAXTUNLEN;
		if(usedsecptr+1<MAXTUNLEN)
			tempint = usedsecptr + 1;
		jumpsize = randjump(tempint);
		startsec = randsectornum[randomnum(usedsecptr+1,NUMSECTORS-11)];
		if(freewarp(startsec))
		{	fromsector = startsec;
			secptrcpy = usedsecptr;
			for(z=0;z<jumpsize;z++)
			{	tempint = randomnum(0,usedsecptr);
				tosector = randsectornum[tempint];
				randsectornum[tempint] = randsectornum[usedsecptr];
				randsectornum[usedsecptr] = tosector;
				usedsecptr--;
				
				secjump(fromsector,tosector);
				secjump(tosector,fromsector);
				fromsector = tosector;
			}
			if(randint <= (100 - DEADENDPERCENT))
			{	do
				{	tosector = randsectornum[randomnum(0,secptrcpy)];
				} while(tosector == startsec || warpsfull(tosector));
				secjump(fromsector,tosector);
				if(randomnum(1,100) <= (100 - ONEWAYJUMPPERCENT))
					secjump(tosector,fromsector);
			}
		}
	}
	printf("done.\n");

	printf("Creating ports!");
	makeports();
	printf("done.\n");

	//Sorts each sector's warps into numeric order
	for(x=0;x<NUMSECTORS;x++)
	{	qsort(sectorlist[x]->sectorptr, numwarps(x), sizeof(struct sector *), compsec);
	}
						
	//Writing data to universe.data file
	printf("Saving universe to file...");	
	file = fopen("./universe.data", "w");
	fileline = malloc(1024*sizeof(char));
	tempstr = malloc(10*sizeof(char));
	for(x=0;x<NUMSECTORS;x++)
	{	sprintf(fileline, "%d", (x+1)) ;
		fileline = strcat(fileline,":");
		for(y=0;y<numwarps(x);y++)
		{	secptr = sectorlist[x]->sectorptr[y];
			sprintf(tempstr, "%d", secptr->number);
			fileline = strcat(fileline,tempstr);
			if(y+1 != numwarps(x))
				fileline = strcat(fileline, ",");
		}
		fileline = strcat(fileline, ":");
		if(sectorlist[x]->beacontext != NULL)
			fileline = strcat(fileline, sectorlist[x]->beacontext);
		fileline = strcat(fileline, ":");
		if(sectorlist[x]->nebulae != NULL)
			fileline = strcat(fileline, sectorlist[x]->nebulae);
		fileline = strcat(fileline, ":\n");
		//Later put in whitespace buffer for saving
		//Not needed until user created beacons put in
		fprintf(file,fileline);
	}
	fclose(file);
	free(fileline);
	free(tempstr);

	//Writing data to ports.data file
	printf("Saving ports to file...");	
	file = fopen("./ports.data", "w");
	fileline = malloc(1024*sizeof(char));
	tempstr = malloc(10*sizeof(char));
	for(x=0;x<NUMPORTS;x++)
	{	
		sprintf(fileline, "%d:%s:%d:%d:%d:%d:%d:%d:%d:%ld:%d:%d", (x+1),
			portlist[x]->name, portlist[x]->location, portlist[x]->maxproduct[0],
			portlist[x]->maxproduct[1], portlist[x]->maxproduct[2],
			portlist[x]->product[0], portlist[x]->product[1], portlist[x]->product[2],
			portlist[x]->credits, portlist[x]->type, portlist[x]->invisible);
		fileline = strcat(fileline, ":");
		len = strlen(fileline);
		for(y=0; y<=99-len; y++)
			strcat(fileline, " ");
		strcat(fileline, "\n");
		fprintf(file,fileline);
	}
	fclose(file);

	printf("done.\nUniverse sucessfully created!\n\n");
	
	return 0;
}

int compsec(const void *cmp1, const void *cmp2)
{	const struct sector *a = *(struct sector **)cmp1;
	const struct sector *b = *(struct sector **)cmp2;

	if(a->number>b->number) return 1;
	if(a->number<b->number) return -1;
	return 0;
}

int randjump(int maxjumplen)
{	if(maxjumplen > 2)
	{	int temprandnum = randomnum(0,2+2+1+maxjumplen);
		//int	temprandnum = 1 + ((int)((double)rand() / ((double) RAND_MAX + 1) * (2+2+1+maxjumplen)));

		if(temprandnum == 0)						return 0;
		if(temprandnum >= 1 && temprandnum <= 3)	return 1;
		if(temprandnum >= 4 && temprandnum <= 6)	return 2;
		if(temprandnum == 7 || temprandnum == 8)	return 3;
		if(temprandnum == 9)						return 4;
		if(temprandnum == 10)						return 5;
		if(temprandnum == 11)						return 6;
		if(temprandnum == 12)						return 7;
		if(temprandnum == 13)						return 8;
		if(temprandnum == 14)						return 9;
		if(temprandnum == 15)				 		return 10;
		return 1;
	}
	return maxjumplen;
}

int randomnum(int min, int max)
{	return (min + ((int)((double)rand() / ((double) RAND_MAX + 1) * (1 + max - min))));
}

void secjump(int from, int to)
{	int y = freewarp(from);
	if(y != -1)
		sectorlist[from]->sectorptr[y] = sectorlist[to];
}

int freewarp(int sector)
{	int x;
	for(x=0;x<configdata->maxwarps;x++)
		if(sectorlist[sector]->sectorptr[x] == NULL)
			return x;
	return -1;
}

int warpsfull(int sector)
{	if(freewarp(sector) == -1)
		return 1;
	return 0;
}

int numwarps(int sector)
{	int x = freewarp(sector);
	if(x == -1)
		return configdata->maxwarps;
	return x;
}

void makeports()
{
   struct port *curport;
	int type=0;
	int loop=0;
	int sector=0;
	char name[50];
	
	for (loop=0; loop<NUMPORTS; loop++)
	{
		curport=(struct port *)malloc(sizeof(struct port));
		curport->number = loop+1;
		curport->name = (char *)malloc(sizeof(struct port));
		strcpy(name, "\0");
		//currently copying Unnamed Port into the names until a port names
		//database can be initialized
		sprintf(name, "Unnamed Port %d", loop+1);
		strcpy(curport->name, name);
		curport->maxproduct[0] = randomnum(2800, 3000);
		curport->maxproduct[1] = randomnum(2800, 3000);
		curport->maxproduct[2] = randomnum(2800, 3000);

		type = randomnum(1, 8);
		curport->type = type;
		curport->product[0]=0;
		curport->product[1]=0;
		curport->product[2]=0;
		curport->credits = 50000;
		curport->invisible = 0;  //Only *special* ports are invisible;
		
		
		switch(type)
		{
			case 1:
				curport->product[2] = curport->maxproduct[2];
				break;
			case 2:
				curport->product[1] = curport->maxproduct[1];
				break;
			case 3:
				curport->product[1] = curport->maxproduct[1];
				curport->product[2] = curport->maxproduct[2];
				break;
			case 4:
				curport->product[0] = curport->maxproduct[0];
				break;
			case 5:
				curport->product[0] = curport->maxproduct[0];
				curport->product[2] = curport->maxproduct[2];
				break;
			case 6:
				curport->product[0] = curport->maxproduct[0];
				curport->product[1] = curport->maxproduct[1];
				break;
			case 7:
				curport->product[0] = curport->maxproduct[0];
				curport->product[1] = curport->maxproduct[1];
				curport->product[2] = curport->maxproduct[2];
				break;
		}
		portlist[loop] = curport;
		curport = NULL;

		//Now for assigning the port to a sector
		sector=randomnum(0,NUMSECTORS-1);
		while (sectorlist[sector]->portptr != NULL)
			sector=randomnum(0,NUMSECTORS-1);
		portlist[loop]->location = sector+1;
	}
}
