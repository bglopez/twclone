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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "universe.h"

int compsec(const void *cmp1, const void *cmp2);

int main(void) 
{	//These are the set-in-stone FedSpace links
	const int sector1[6]={2,3,4,5,6,7};
	const int sector2[6]={1,3,7,8,9,10};
	const int sector3[3]={1,2,4};
	const int sector4[3]={1,3,5};
	const int sector5[3]={1,4,6};
	const int sector6[3]={1,5,7};
	const int sector7[4]={1,2,6,8};
	const int sector8[2]={2,7};
	const int sector9[2]={2,10};
	const int sector10[2]={2,9};
	const int MAXTUNLEN = 5; //This is the max length of tunnels and dead ends.
	
	int numsectors = 500, x, y, z, randint, tempsec, secnumpostion = 0;
	int arraysize, jumpsize, finalsec, randsec, jumpfrom, tempjump, jumpsizemax;	int usedsecptr = 0, randsecptr = numsectors - 11, secptrnum[numsectors];
	int randsectornum[numsectors-10], usedsector[numsectors-10];
	int sixjumpsize = (int) numsectors * .05;
	FILE *filetowrite;
	char *tempwrite, *tempwrite2;
	struct sector **sectorlist, *sectorpointer, *sortptr[6];

	printf("\nCreating sector array...\n");
	sectorlist = malloc(numsectors * sizeof(struct sector *));
	for(x = 0; x < numsectors; x++)
		sectorlist[x] = malloc(sizeof(struct sector));

	//Fills in the randsectornum array with numbers 10 to (numsectors - 1)
	for(x=0;x<numsectors-10;x++)
		randsectornum[x]=x+10;
	
	//Seed our randomizer
	srand((unsigned long) time(NULL));
	
	//Initalize sectorlist with data
	for(x=0;x<numsectors;x++)
	{	sectorlist[x]->number = x+1;
		secptrnum[x]=0;
	}
	
	printf("\nCreating Fedspace...\n");
	//Sets up Fed Space
	for(x=0;x<6;x++)
	{	sectorlist[0]->sectorptr[x]=sectorlist[sector1[x]-1];
		secptrnum[0]++;
	}
	for(x=0;x<6;x++)
	{	sectorlist[1]->sectorptr[x]=sectorlist[sector2[x]-1];
		secptrnum[1]++;
	}
	for(x=0;x<3;x++)
	{	sectorlist[2]->sectorptr[x]=sectorlist[sector3[x]-1];
		secptrnum[2]++;
	}
	for(x=0;x<3;x++)
	{	sectorlist[3]->sectorptr[x]=sectorlist[sector4[x]-1];
		secptrnum[3]++;
	}
	for(x=0;x<3;x++)
	{	sectorlist[4]->sectorptr[x]=sectorlist[sector5[x]-1];
		secptrnum[4]++;
	}
	for(x=0;x<3;x++)
	{	sectorlist[5]->sectorptr[x]=sectorlist[sector6[x]-1];
		secptrnum[5]++;
	}
	for(x=0;x<4;x++)
	{	sectorlist[6]->sectorptr[x]=sectorlist[sector7[x]-1];
		secptrnum[6]++;
	}
	for(x=0;x<2;x++)
	{	sectorlist[7]->sectorptr[x]=sectorlist[sector8[x]-1];
		secptrnum[7]++;
	}
	for(x=0;x<2;x++)
	{	sectorlist[8]->sectorptr[x]=sectorlist[sector9[x]-1];
		secptrnum[8]++;
	}
	for(x=0;x<2;x++)
	{	sectorlist[9]->sectorptr[x]=sectorlist[sector10[x]-1];
		secptrnum[9]++;
	}
	
	printf("\nRandomly picking sector numbers...\n");
	//Randomly creates sector numbers to use:
	for(x=numsectors-11;x>0;x--)
	{	randint = (int)((double)rand() / ((double) RAND_MAX + 1) * (x + 1));
		tempsec = randsectornum[randint];
		randsectornum[randint] = randsectornum[x];
		randsectornum[x] = tempsec;
	}
	
	printf("\nSetting up FedSpace to 6J jumps...\n");
	//Sets up 13 jumps from Fed Space to 6jump sectors 
	for(x=0;x<=13;x++)
	{	do
		{	randint = 3 + ((int)((double)rand() / ((double) RAND_MAX + 1) * (1 + 10 - 3)));
		} while(secptrnum[randint] >= 6);
		sectorlist[randint]->sectorptr[secptrnum[randint]] = sectorlist[randsectornum[x]];
		sectorlist[randsectornum[x]]->sectorptr[secptrnum[randsectornum[x]]] = sectorlist[randint];
		secptrnum[randint]++;
		secptrnum[randsectornum[x]]++;
	}

	printf("\nSetting up rest of 6J jumps...\n");
	//Sets up rest of links for the 6jump sectors (the meat and potatoes)
	for(x=0;x<sixjumpsize;x++)
	{	for(y=secptrnum[randsectornum[x]];y<MAX_WARPS_PER_SECTOR;y++)
		{	randint = 1 + ((int)((double)rand() / ((double) RAND_MAX + 1) * (5)));
			jumpsize = 1 + ((int)((double)rand() / ((double) RAND_MAX + 1) * (MAXTUNLEN)));
			jumpfrom=randsectornum[x];
			tempjump=jumpfrom;
			for(z=0;z<jumpsize;z++)
			{	tempsec = sixjumpsize + ((int)((double)rand() / ((double) RAND_MAX + 1) * (1 + randsecptr - sixjumpsize)));
				randsec = randsectornum[tempsec];
				usedsector[usedsecptr] = randsectornum[tempsec];
				usedsecptr++;
				randsectornum[tempsec] = randsectornum[randsecptr];
				randsecptr--;
				
				sectorlist[tempjump]->sectorptr[secptrnum[tempjump]] = sectorlist[randsec];
				sectorlist[randsec]->sectorptr[secptrnum[randsec]] = sectorlist[tempjump];
				secptrnum[tempjump]++;
				secptrnum[randsec]++;
				tempjump=randsec;
			}
			if(randint<4)
			{	do
				{	tempsec = ((int)((double)rand() / ((double) RAND_MAX + 1) * (sixjumpsize)));
					finalsec = randsectornum[tempsec];
				} while(finalsec == jumpfrom);
				sectorlist[tempjump]->sectorptr[secptrnum[tempjump]] = sectorlist[finalsec];
				secptrnum[tempjump]++;
				if((1+(int)((double)rand()/((double) RAND_MAX + 1) * 24)) < 16 && secptrnum[finalsec]<6)
				{	sectorlist[finalsec]->sectorptr[secptrnum[finalsec]] = sectorlist[tempjump];
					secptrnum[finalsec]++;
				}
			}
		}
	}
	
	for(x=0;x<sixjumpsize;x++)
	{	randsectornum[x] = randsectornum[randsecptr];
		randsecptr--;
	}

	printf("\nSetting up rest of universe...using up leftover sector numbers...\n");	
	while(randsecptr>=0)  //finishes up creating other sector links...
	{	randint = 1 + ((int)((double)rand() / ((double) RAND_MAX + 1) * (5)));
		jumpsizemax=6;
		if(randsecptr<6)
			jumpsizemax = randsecptr;
		jumpsize = 1 + ((int)((double)rand() / ((double) RAND_MAX + 1) * (jumpsizemax)));
		x = ((int)((double)rand() / ((double) RAND_MAX + 1) * (usedsecptr)));
		if(secptrnum[x]<6)
		{	jumpfrom=usedsector[x];
			for(z=0;z<jumpsize;z++)
			{	tempsec = ((int)((double)rand() / ((double) RAND_MAX + 1) * (randsecptr + 1)));
				randsec = randsectornum[tempsec];
				usedsector[usedsecptr] = randsectornum[tempsec];
				usedsecptr++;
				randsectornum[tempsec] = randsectornum[randsecptr];
				randsecptr--;
			
				sectorlist[jumpfrom]->sectorptr[secptrnum[jumpfrom]] = sectorlist[randsec];
				sectorlist[randsec]->sectorptr[secptrnum[randsec]] = sectorlist[jumpfrom];
				secptrnum[jumpfrom]++;
				secptrnum[randsec]++;
				jumpfrom=randsec;
			}
			if(randint<4)
			{	do
				{	finalsec = ((int)((double)rand() / ((double) RAND_MAX + 1) * (finalsec)));
					finalsec = usedsector[finalsec];
				} while(finalsec == usedsector[x]);
				sectorlist[jumpfrom]->sectorptr[secptrnum[jumpfrom]] = sectorlist[finalsec];
				secptrnum[jumpfrom]++;
				if((1+(int)((double)rand()/((double) RAND_MAX + 1) * 24)) < 16 && secptrnum[finalsec]<6)
				{	sectorlist[finalsec]->sectorptr[secptrnum[finalsec]] = sectorlist[jumpfrom];
					secptrnum[finalsec]++;
				}
			}
		}
	}

	for(x=0;x<numsectors;x++)
		sectorlist[x]->sectorptr[secptrnum[x]] = NULL;

	//Sorts warp pointer array from smallest to largest
	for(x=0;x<6;x++)
		sortptr[x] = malloc(sizeof(struct sector *));
	for(x=0;x<numsectors;x++)
	{	for(y=0;y<secptrnum[x];y++)
			sortptr[y]=sectorlist[x]->sectorptr[y];
		qsort(sectorlist[x]->sectorptr, secptrnum[x], sizeof(struct sector *), compsec);
	}
						
	//Writing data to universe.data file
	printf("\nSaving universe to file...\n");	
	filetowrite = fopen("universe.data", "w");
	tempwrite = malloc(256*sizeof(char));
	tempwrite2 = malloc(10*sizeof(char));
	for(x=0;x<numsectors;x++)
	{	sprintf(tempwrite, "%d", (x+1)) ;
		strcat(tempwrite,":");
		for(y=0;y<secptrnum[x];y++)
		{	sectorpointer = sectorlist[x]->sectorptr[y];
			sprintf(tempwrite2, "%d", sectorpointer->number);
			strcat(tempwrite,tempwrite2);
			if(y+1 != secptrnum[x])
				tempwrite = strcat(tempwrite, ",");
		}
		tempwrite = strcat(tempwrite, ":::\n");
		fprintf(filetowrite,tempwrite);
	}
	fclose(filetowrite);
}

int compsec(const void *cmp1, const void *cmp2)
{	const struct sector *a = *(struct sector **)cmp1;
	const struct sector *b = *(struct sector **)cmp2;

	if(a->number>b->number) return 1;
	if(a->number<b->number) return -1;
	return 0;
}

