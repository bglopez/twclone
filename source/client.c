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

/* twclone Program. client.c
 *
 * This program interfaces with the server and producs nice looking output
 * for the user.
 *   
 * $Revision: 1.34 $
 * Last Modified: $Date: 2003-11-13 00:45:01 $
 */

/* Normal Libary Includes */
#include <stdio.h>
#include <termios.h>		//for inv. pw
#include <string.h>
#include <stdlib.h>		//for strtoul
#include <sys/time.h>
#include <sys/poll.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

struct timeval t, end;
static char CVS_REVISION[50] = "$Revision: 1.34 $\0";
static char LAST_MODIFIED[50] = "$Date: 2003-11-13 00:45:01 $\0";

//these are for invisible passwords
static struct termios orig, new;
static int peek = -1;

/* My Headers */
#include "ansi.h"
#include "common.h"
#include "parse.h"
#include "client.h"
#include "portinfo.h"

int
main (int argc, char *argv[])
{
    char *goofey = NULL;		//For holding input from stdin
    char *mickey = NULL;
    int sector = 1;
    int loop = 1, counter;	//Loop counters
    int port = 1234;		//Port number of the server
    int sockid;			//Socket ID
    char *buffer;
    enum prompts ptype;
    struct sector *cursector = NULL;	//Stores stuff about the current sector
    struct player *curplayer = NULL;	//For the player logging in
    struct ship *curship = NULL;	//For the current players ship

    gettimeofday (&t, 0);

    /*
       Network initilization
     */
    //parse command line
    switch (argc)
    {
    case 3:			//specified port and host
        //change the string to a long
        port = strtoul (argv[2], NULL, 10);
        if (port == 0)		//if it wasn't possible to change to an int
        {
            //quit, and tell the user how to use us
            printf ("usage:  %s server [port_num]\n", argv[0]);
            exit (-1);
        }
        printf ("port %d specified\n", port);
    case 2:			//specified host only
        break;
    default:			//something else entirely
        printf ("usage: %s server [port_num]\n", argv[0]);
        exit (-1);
    };

    printf ("Connecting to %s on port %d...", argv[1], port);
    fflush (stdout);

    //setting up the sockaddt pointing to the server
    sockid = init_clientnetwork (argv[1], port);
    printf (" done\n");

    /*
       Network initilization
     */

    /*
       make sure that buffer has been allocated in both
       cases, everything will end up in buffers original
       memory
       both of these return -1 if they fail, 
       and have already called perror

       to send stuff to the server, call like this:
       sendinfo(int sockid, char *buffer);

       to recieve stuff from the server, call like this:
       recvinfo(int sockid, char *buffer);
     */

    dologin (sockid);
    //recvinfo(sockid, buffer);    //Gets rid of the stuff login sends
    buffer = NULL;
    printwelcome ();
    whosplaying (sockid);
    printf ("\n");
    if ((cursector = (struct sector *) malloc (sizeof (struct sector))) != NULL)
    {				//Need memory
        cursector->players = NULL;
        cursector->ships = NULL;
        cursector->ports = NULL;
        cursector->planets = NULL;
        cursector->beacontext = NULL;	//Initialization of pointers
        cursector->nebulae = NULL;
        if ((curplayer =
                    (struct player *) malloc (sizeof (struct player))) == NULL)
        {
            perror ("main: Unable to allocate enough memory for 1 player.");
            exit (-1);
        }
        curplayer->name = NULL;
        curplayer->title = NULL;	//Initialize more pointers
        curplayer->next = NULL;
        if ((curship = (struct ship *) malloc (sizeof (struct ship))) == NULL)
        {
            perror ("main: Unable to allocate enough memory for your ship.");
            exit (-1);
        }
        curship->name = NULL;
        curship->type = NULL;
        curship->next = NULL;	//Again more pointer initialization
        curplayer->pship = curship;
        getmyinfo (sockid, curplayer);
        sector = getsectorinfo (sockid, cursector);
        printsector (cursector);
        for (; loop;)
        {
            ptype = command;	//Main command type of prompt
            if (goofey != NULL)
                free (goofey);
            goofey = prompttype (ptype, sector, sockid);
            if (isdigit (*(goofey + 0)) != 0)
            {
                sector = movesector (goofey, sockid, sector, cursector);
            }
            else if (isspace (*(goofey + 0)) == 0)
            {
                switch (*(goofey + 0))
                {
                case 'q':
                case 'Q':
                    printf ("\x1B[5;31m<Quit>?%s ", KNRM);
                    ptype = quit;
                    if (getyes (prompttype (ptype, 0, sockid)))
                    {
                        loop = 0;
                        sendinfo (sockid, "QUIT");
                    }
                    printf ("\n%s", KNRM);
                    break;
                case 'd':
                case 'D':
                    printf ("\n%s%s<Re-Display>%s\n", KBBLU, KFWHT, KNRM);
                    sector = getsectorinfo (sockid, cursector);
                    printsector (cursector);
                    break;
                case 'i':
                case 'I':
                    getmyinfo (sockid, curplayer);
                    printmyinfo (curplayer);
                    break;
                case 'p':
                case 'P':
                    ptype = pt_port;
                    sector = getsectorinfo (sockid, cursector);
                    if (cursector->ports == NULL)
                    {
                        printf ("There's no port in this sector!");
                        break;
                    }
                    if (cursector->ports->type == 0)
                    {
                        //printf("Porting at Class 0 Ports currently unavailable");
								//Upgrades are called from 
                    }
						  //This next bit is cheating but it works!

                    mickey = prompttype (ptype, cursector->ports->type, sockid);
                    switch (*(mickey + 0))
                    {
                    		case 'q':
                    		case 'Q':
                        	break;
                    		case 't':
                    		case 'T':
                        	doporting (sockid, curplayer);
									//Until I find out why it seg faults this is here
									getmyinfo(sockid, curplayer);
                        	break;
						  		case 's':
						  		case 'S':
									printf("Landing at Stardock current disabled! But you can do upgrades!");
									do_ship_upgrade(sockid, curplayer);
									break;
								default:
									break;
                    }
                    break;	//Porting ain't done yet.
                case 'm':
                case 'M':	//Shorthand for this command is to type
                    //Just the sector number
                    printf ("\n%sWarps to Sector(s) %s:%s  %d", KLTGRN, KLTYLW,
                            KLTCYN, cursector->warps[0]);

                    for (counter = 1; counter < MAX_WARPS_PER_SECTOR; counter++)
                    {
                        if (cursector->warps[counter] == 0)
                            break;
                        printf (" %s- %s%d", KGRN, KLTCYN,
                                cursector->warps[counter]);
                    }

                    ptype = move;
                    goofey = prompttype (ptype, sector, sockid);
                    if (isdigit (*(goofey + 0)) != 0)
                    {
                        sector = movesector (goofey, sockid, sector, cursector);
                    }
                    break;
                case '?':
                    printhelp ();
                    break;
                case '`':
                    fedcommlink (sockid);
                    break;
                case '#':
                    whosplaying (sockid);
                    break;
                case '~':
                    if (strncmp (goofey, "~debug", 6) == 0)
                    {
                        printf ("\nEntering debug mode.");
                        debugmode (sockid);
                    }
                    getmyinfo (sockid, curplayer);
                    sector = getsectorinfo (sockid, cursector);
                    printsector (cursector);
                    break;
                default:
                    printf ("\nSorry that option is not supported yet.");
                    break;
                }
            }
        }
        return (0);
    }
    else
        return (-1);

}

void
printwelcome ()
{
    printf ("\n%sWelcome to TWClone.", KGRN);
    printf ("\n\n%s", CVS_REVISION);
    printf ("\nClient Last Modified on: %s", LAST_MODIFIED);
    printf ("\n");
    printf ("\nTWClone Hompage: %shttp://twclone.sourceforge.net%s", KLTCYN,
            KGRN);
    printf ("\nCurrent Release: 0.12");
    printf ("\n");
}

void
fedcommlink (int sockid)
{
    char message[BUFF_SIZE], buffer[BUFF_SIZE];

    printf ("\n\n%sFederation comm-link:", KGRN);
    printf ("\n\n%s`%s", KMAG, KYLW);
    fgets (message, BUFF_SIZE, stdin);
    message[strcspn (message, "\n")] = '\0';
    strcpy (buffer, "FEDCOMM ");
    strcat (buffer, message);
    strcat (buffer, ":\0");
    sendinfo (sockid, buffer);
    recvinfo (sockid, buffer);

}

void
whosplaying (int sockid)
{
    char *buffer = (char *) malloc (BUFF_SIZE * sizeof (char));
    char *temp = (char *) malloc (BUFF_SIZE * sizeof (char));
    int pnumb = 0;

    char *name = (char *) malloc (BUFF_SIZE * sizeof (char));
    int exper = 0;
    int align = 0;
    int rank;

    strcpy (temp, "\0");
    printf
    ("\n%s%s                             Who's Playing                             %s",
     KBBLU, KFWHT, KNRM);
    printf ("\n");
    strcpy (buffer, "ONLINE");
    sendinfo (sockid, buffer);
    recvinfo (sockid, buffer);
    popstring (buffer, temp, ":", BUFF_SIZE);
    while (strlen (temp) > 0)
    {
        pnumb = popint (temp, ",");
        sprintf (buffer, "PLAYERINFO %d:", pnumb);
        sendinfo (sockid, buffer);
        recvinfo (sockid, buffer);
        popstring (buffer, name, ":", BUFF_SIZE);
        exper = popint (buffer, ":");
        align = popint (buffer, ":");
		  if (exper != 0)
        	rank = (int)(log (exper) / log (2));	//Since exp ranks go by mod 2..
		  else
			rank = 0;
        if (align < 0)
            printf ("\n%s%s %s", KRED, evil_ranks[rank], name);
        else
            printf ("\n%s%s %s", KLTCYN, good_ranks[rank], name);
    }

    free (buffer);
    free (temp);
	 free(name);
}

void debugmode (int sockid)
{
    char buffer[BUFF_SIZE];

    printf ("\nAnything you type will be sent directly to the server");
    printf ("\nType 'exit' to exit out of debug mode");
    printf ("\nThe BAD server response from exit is automatically discarded");
    printf ("\n");
    printf ("\n>");
    fgets (buffer, BUFF_SIZE, stdin);
    buffer[strcspn (buffer, "\n")] = '\0';

    if (sendinfo (sockid, buffer) == -1)
        return;
    printf ("\nSent '%s'\n", buffer);
    do
    {
        if (recvinfo (sockid, buffer) == -1)
            exit (-1);

        printf ("\nServer response '%s'", buffer);
        printf ("\n>");

        fgets (buffer, BUFF_SIZE, stdin);
        buffer[strcspn (buffer, "\n")] = '\0';

        if (sendinfo (sockid, buffer) == -1)
            exit (-1);

        printf ("\nSent '%s'\n", buffer);

    }
    while (strcmp (buffer, "exit") != 0);
    recvinfo (sockid, buffer);	//Gets rid of the BAD from exit
}

int getintstuff ()
{
    char ch;
    int i;
    ch = getchar ();
    i = (int) ch;
    if (isdigit (i) != 0)
        return i;
    else
        return -1;
}

void junkline ()
{
    int loop = 1;
    char ch;

    for (; loop;)
    {
        if ((ch = getchar ()) == '\n')
        {
            loop = 0;
        }
    }
}

void printhelp ()
{
    printf
    ("\n%s|================================%stwclone%s================================|",
     KGRN, KLTBLU, KGRN);
    printf
    ("\n%s|                            %sGlobal Commands%s                            |",
     KGRN, KMAG, KGRN);
    printf
    ("\n%s|                            %s=%s-%s=%s-%s=%s-%s=%s-%s=%s-%s=%s-%s=%s-%s=%s                            |",
     KGRN, KLTYLW, KGRN, KLTYLW, KGRN, KLTYLW, KGRN, KLTYLW, KGRN, KLTYLW,
     KGRN, KLTYLW, KGRN, KLTYLW, KGRN, KLTYLW, KGRN);
    printf
    ("\n%s| %s<%s`%s>%s Fed. Comm-Link    %s<%s'%s>%s Sub-Space Radio  %s<%s=%s>%s Hailing Frequencies    %s|",
     KGRN, KMAG, KGRN, KMAG, KLTRED, KMAG, KGRN, KMAG, KLTRED, KMAG, KGRN,
     KMAG, KLTRED, KGRN);
    printf
    ("\n%s| %s<%s#%s>%s Who's Playing     %s<%s*%s>%s List All Globals %s<%s/%s>%s Quick-stats            %s|",
     KGRN, KMAG, KGRN, KMAG, KLTRED, KMAG, KGRN, KMAG, KLTRED, KMAG, KGRN,
     KMAG, KLTRED, KGRN);
    printf
    ("\n%s| --------------------------------------------------------------------- |",
     KGRN);
    printf
    ("\n%s|        %sNavigation            Computer              Tactical           %s|",
     KGRN, KMAG, KGRN);
    printf
    ("\n%s|        %s=%s-%s=%s-%s==%s-%s=%s-%s=            %s=%s-%s=%s--%s=%s-%s=              %s=%s-%s=%s--%s=%s-%s=           %s|",
     KGRN, KLTYLW, KGRN, KLTYLW, KGRN, KLTYLW, KGRN, KLTYLW, KGRN, KLTYLW,
     KLTYLW, KGRN, KLTYLW, KGRN, KLTYLW, KGRN, KLTYLW, KLTYLW, KGRN, KLTYLW,
     KGRN, KLTYLW, KGRN, KLTYLW, KGRN);
    printf
    ("\n%s| %s<%sD%s>%s Re-Display Sector %s<%sC%s>%s Onboard Computer %s<%sA%s>%s Attack Enemy Ship      %s|",
     KGRN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN,
     KMAG, KLTCYN, KGRN);
    printf
    ("\n%s| %s<%sP%s>%s Port and Trade    %s<%sX%s>%s Transporter Pad  %s<%sE%s>%s Sub-space EtherProbe   %s|",
     KGRN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN,
     KMAG, KLTCYN, KGRN);
    printf
    ("\n%s| %s<%sM%s>%s Move to a Sector  %s<%sI%s>%s Ship Information %s<%sF%s>%s Take or Leave Fighters %s|",
     KGRN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN,
     KMAG, KLTCYN, KGRN);
    printf
    ("\n%s| %s<%sL%s>%s Land on a Planet  %s<%sT%s>%s Corporate Menu   %s<%sG%s>%s Show Deployed Fighters %s|",
     KGRN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN,
     KMAG, KLTCYN, KGRN);
    printf
    ("\n%s| %s<%sS%s>%s Long Range Scan   %s<%sU%s>%s Use Genesis Torp %s<%sH%s>%s Handle Space Mines     %s|",
     KGRN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN,
     KMAG, KLTCYN, KGRN);
    printf
    ("\n%s| %s<%sR%s>%s Release Beacon    %s<%sJ%s>%s Jettison Cargo   %s<%sK%s>%s Show Deployed Mines    %s|",
     KGRN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN,
     KMAG, KLTCYN, KGRN);
    printf
    ("\n%s| %s<%sW%s>%s Tow SpaceCraft    %s<%sB%s>%s Interdictor Ctrl %s<%sO%s>%s Starport Construction  %s|",
     KGRN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN,
     KMAG, KLTCYN, KGRN);
    printf
    ("\n%s| %s<%sN%s>%s Move to NavPoint  %s<%s %s>%s                  %s<%sY%s>%s Set NavPoints          %s|",
     KGRN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN,
     KMAG, KLTCYN, KGRN);
    printf
    ("\n%s| %s<%sQ%s>%s Quit and Exit     %s<%s %s>%s                  %s<%sV%s>%s View Game Status       %s|",
     KGRN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN, KMAG, KLTCYN, KMAG, KGRN,
     KMAG, KLTCYN, KGRN);
    printf
    ("\n%s|==============================%s0.12 alpha%s===============================|",
     KGRN, KLTRED, KGRN);

}

void getmyinfo (int sockid, struct player *curplayer)
{
    char *buffer = (char *) malloc (BUFF_SIZE);
    int position = 1;
    char sname[50], type[50];
    char pname[70], title[50];

    strcpy (buffer, "MYINFO");
    sendinfo (sockid, buffer);
    *buffer = '\0';
    recvinfo (sockid, buffer);
    /* This is in the same order as in client.c */
    curplayer->number = popint (buffer + position, ":");
    popstring (buffer + position, pname, ":", 70);
    curplayer->shipnumb = popint (buffer + position, ":");
    curplayer->exper = popint (buffer + position, ":");
    curplayer->align = popint (buffer + position, ":");
    curplayer->turns = popint (buffer + position, ":");
    curplayer->credits = popint (buffer + position, ":");
    curplayer->pship->number = popint (buffer + position, ":");
    popstring (buffer + position, sname, ":", 50);
    popstring (buffer + position, type, ":", 50);
    curplayer->pship->fighters = popint (buffer + position, ":");
    curplayer->pship->shields = popint (buffer + position, ":");
    curplayer->pship->holds = popint (buffer + position, ":");
    curplayer->pship->colonists = popint (buffer + position, ":");
    curplayer->pship->equipment = popint (buffer + position, ":");
    curplayer->pship->organics = popint (buffer + position, ":");
    curplayer->pship->ore = popint (buffer + position, ":");
    curplayer->pship->ownedby = popint (buffer + position, ":");
    curplayer->pship->location = popint (buffer + position, ":");
    curplayer->pship->turnsperwarp = popint (buffer + position, ":");

	 if (curplayer->exper != 0)
	 {
    	curplayer->rank = log (curplayer->exper) / log (2);	//Since exp ranks go by mod 2...
	 }
	 else
		curplayer->rank = 0;
    curplayer->blownup = 0;
    curplayer->pship->ported = 0;
    curplayer->pship->kills = 0;
    curplayer->pship->emptyholds = curplayer->pship->holds -
    curplayer->pship->equipment - curplayer->pship->organics -
    curplayer->pship->ore - curplayer->pship->colonists;
    if (curplayer->align < 0)
    {
        strcpy (title, KRED);
        strcat (title, evil_ranks[curplayer->rank]);
    }
    else
    {
        strcpy (title, KLTCYN);
        strcat (title, good_ranks[curplayer->rank]);
    }
    if (curplayer->name == NULL)
        curplayer->name = (char *) malloc (strlen (pname) + 1);
    if (curplayer->title == NULL)
        curplayer->title = (char *) malloc (strlen (title) + 1);
    if (curplayer->pship->name == NULL)
        curplayer->pship->name = (char *) malloc (strlen (sname) + 1);
    if (curplayer->pship->type == NULL)
        curplayer->pship->type = (char *) malloc (strlen (type) + 1);
    strncpy (curplayer->name, pname, strlen (pname) + 1);
    strncpy (curplayer->title, title, strlen (title) + 1);
    strncpy (curplayer->pship->name, sname, strlen (sname) + 1);
    strncpy (curplayer->pship->type, type, strlen (type) + 1);
	 free(buffer);
    return;
}

void printmyinfo (struct player *curplayer)
{

    printf ("\n%s%s<Info>%s", KBBLU, KFWHT, KNRM);
    printf ("\n");
    printf ("\n%sTrader Name    %s:%s %s %s", KMAG, KLTYLW, KGRN,
            curplayer->title, curplayer->name);
    printf ("\n%sRank and Exp   %s:%s %ld %spoints%s,%s Alignment%s=%s%ld",
            KMAG, KLTYLW, KLTCYN, curplayer->exper, KGRN, KLTYLW, KGRN, KLTYLW,
            KLTCYN, curplayer->align);
    printf ("\n%sTimes Blown Up %s:%s %d", KMAG, KLTYLW, KGRN,
            curplayer->blownup);
    //Put Corpinfo here!
    printf ("\n%sShip Name      %s:%s %s", KMAG, KLTYLW, KGRN,
            curplayer->pship->name);
    printf ("\n%sShip Info      %s:%s %s %sPorted%s=%s%d %sKills%s=%s%d", KMAG,
            KLTYLW, KGRN, curplayer->pship->type, KMAG, KLTYLW, KLTCYN,
            curplayer->pship->ported, KMAG, KLTYLW, KLTCYN,
            curplayer->pship->kills);
    printf ("\n%sDate Built     %s:%s Unknown", KMAG, KLTYLW, KGRN);
    printf ("\n%sTurns to Warp  %s:%s %d", KMAG, KLTYLW, KGRN,
            curplayer->pship->turnsperwarp);
    printf ("\n%sCurrent Sector %s:%s %d", KMAG, KLTYLW, KLTCYN,
            curplayer->pship->location);
    printf ("\n%sTurns left     %s:%s %d", KMAG, KLTYLW, KLTCYN,
            curplayer->turns);
    printf ("\n%sTotal Holds    %s:%s %d - %s ", KMAG, KLTYLW, KLTCYN,
            curplayer->pship->holds, KGRN);
    if (curplayer->pship->ore != 0)
        printf ("%sFuel Ore%s=%s%d ", KGRN, KLTYLW, KLTCYN,
                curplayer->pship->ore);
    if (curplayer->pship->organics != 0)
    {
        printf ("%sOrganics%s=%s%d ", KGRN, KLTYLW, KLTCYN,
                curplayer->pship->organics);
    }
    if (curplayer->pship->equipment != 0)
    {
        printf ("%sEquipment%s=%s%d ", KGRN, KLTYLW, KLTCYN,
                curplayer->pship->equipment);
    }
    if (curplayer->pship->equipment != 0)
    {
        printf ("%sColonists%s=%s%d ", KGRN, KLTYLW, KLTCYN,
                curplayer->pship->colonists);
    }
    printf ("%sEmpty%s=%s%d", KGRN, KLTYLW, KLTCYN,
            curplayer->pship->emptyholds);
    printf ("\n%sFighters       %s:%s %d", KMAG, KLTYLW, KLTCYN,
            curplayer->pship->fighters);
    printf ("\n%sShield points  %s:%s %d", KMAG, KLTYLW, KLTCYN,
            curplayer->pship->shields);
    printf ("\n%sCredits        %s:%s %d", KMAG, KLTYLW, KLTCYN,
            curplayer->credits);
    printf ("\n%s", KNRM);

}

void psinfo (int sockid, int pnumb, struct player *p)
{
    char *buffer = (char *) malloc (BUFF_SIZE);
    char *temp = (char *) malloc (70);
    int position = 1;
    char *intptr = (char *) malloc (5);
    struct ship *curship;
    char name[50], type[50];
    char pname[70], title[50];

    *buffer = '\0';
    *temp = '\0';
    p->next = NULL;
    p->pship = NULL;
    strcpy (buffer, "PLAYERINFO ");
    sprintf (intptr, "%d", pnumb);
    strcat (buffer, intptr);
    strcat (buffer, ":");
    sendinfo (sockid, buffer);
    *buffer = '\0';
    recvinfo (sockid, buffer);
    if (strncmp ("BAD", buffer, 3) == 0)
    {
        printf ("\nBAD Server response.");
        free (buffer);
        free (temp);
        free (p);
		  free(intptr);
        p = NULL;
        return;
    }
    popstring (buffer + position, pname, ":", 70);
    p->exper = popint (buffer + position, ":");
    p->align = popint (buffer + position, ":");
    if ((curship = (struct ship *) malloc (sizeof (struct ship))) != NULL)
    {
        curship->name = NULL;
        curship->type = NULL;
        curship->next = NULL;
        curship->number = popint (buffer + position, ":");
        *buffer = '\0';
        strcpy (buffer, "SHIPINFO ");
        sprintf (intptr, "%d", curship->number);
        strcat (buffer, intptr);
        strcat (buffer, ":");
        sendinfo (sockid, buffer);
        *(buffer + 0) = '\0';
        position = 1;
        recvinfo (sockid, buffer);
        curship->ownedby = popint (buffer + position, ":");
        if (curship->ownedby != pnumb)
        {
            perror ("psinfo: Mismatched player number -");
            //exit(-1);           //Cheating bad! Cheating BAD!
        }
        popstring (buffer + position, name, ":", 70);
        popstring (buffer + position, type, ":", 70);
        curship->fighters = popint (buffer + position, ":");
        curship->shields = popint (buffer + position, ":");
		  if (p->exper != 0)
        	p->rank = (int)(log (p->exper) / log (2));	//Since exp ranks go by mod 2...
		  else
			p->rank = 0;
        if (p->align < 0)
        {
            strcpy (title, KRED);
            strcat (title, evil_ranks[p->rank]);
        }
        else
        {
            strcpy (title, KLTCYN);
            strcat (title, good_ranks[p->rank]);
        }
        if (curship->name == NULL)
            curship->name = (char *) malloc (sizeof(char)*(strlen (name) + 1));
        if (curship->type == NULL)
            curship->type = (char *) malloc (sizeof(char)*(strlen (type) + 1));
        if (p->title == NULL)
            p->title = (char *) malloc (sizeof(char)*(strlen (title) + 1));
        if (p->name == NULL)
            p->name = (char *) malloc (sizeof(char)*(strlen (pname) + 1));
        strncpy (curship->name, name, strlen (name) + 1);
        strncpy (curship->type, type, strlen (type) + 1);
        strncpy (p->title, title, strlen (title) + 1);
        strncpy (p->name, pname, strlen (pname) + 1);

        free (buffer);
        free (temp);
		  free(intptr);
        p->pship = curship;
    }
    else
    {
        printf ("\nUnable to allocate memory for ship.");
        free (buffer);
        free (temp);
        free (p);
		  free(intptr);
        free (curship);
        p = NULL;
        return;
    }
    return;
}

int getsectorinfo (int sockid, struct sector *cursector)
{
    int length, position, len, pos;
    int counter;
    char *buff = NULL;
    char buffer[BUFF_SIZE] = "\0";	//For buffer storage
    char tempbuf[BUFF_SIZE] = "\0";	//For temporary buffer storage
    char temp[1024] = "\0";		//To fix a wierd error with psinfo.
    //Change the size of temp if you need more
    //than 20 or so players in a sector at one
    //time
    char portname[50] = "\0";
    int porttype = 0;
    int tempplayer;
    struct player *curplayer = NULL, *place = NULL;
    struct port *curport = NULL;
    struct planet *curplanet = NULL, *pplace = NULL;
    char beacon[50], nebulae[50];

    for (counter = 0; counter <= MAX_WARPS_PER_SECTOR; counter++)
        cursector->warps[counter] = 0;

    len = pos = 0;		//For parsing within parsed stuff
    length = 0;
    buff = buffer;
    sendinfo (sockid, "DESCRIPTION");
    strcpy (buff, "\0");
    recvinfo (sockid, buff);
    position = 1;			//Gets rid of first : from description
    cursector->number = popint (buffer + position, ":");
    popstring (buffer + position, tempbuf, ":", MAX_NAME_LENGTH);
    length = strlen (tempbuf);
    cursector->planets = NULL;
    for (counter = 0; counter <= MAX_WARPS_PER_SECTOR; counter++)
    {
        if ((len = strcspn (tempbuf + pos, ",")) == 0)
        {			//I banish thee foul errors
            return cursector->number;	//from this here function!
        }			//Eeek! It backfired!
        cursector->warps[counter] = popint (tempbuf + pos, ",");
        if (strlen (tempbuf) == 0)
            counter = MAX_WARPS_PER_SECTOR + 1;
    }
    if ((length = strcspn (buff + position, ":")) == 0)
    {				//Because popstring acts wierd
        strcpy (beacon, "\0");	//If there isn't anything there.
        position++;
    }
    else
        popstring (buffer + position, beacon, ":", 50);
    if ((length = strcspn (buff + position, ":")) == 0)
    {
        strcpy (nebulae, KBLU);
        strcat (nebulae, "uncharted space");
        position++;
    }
    else
    {
        strcpy (nebulae, KLTGRN);
        popstring (buffer + position, tempbuf, ":", MAX_NAME_LENGTH);
        strncat (nebulae, tempbuf, length);
        if (strncmp (tempbuf, "Uncharted Space", 15) == 0)
        {
            strcpy (nebulae, KBLU);
            strcat (nebulae, "uncharted space");
        }
    }
	 //This checks for the Port name!
    if ((length = strcspn (buff + position, ":")) == 0)	//If no port
    {
        cursector->ports = NULL;	//Then no port!
        position++;
    }
    else
        popstring (buffer + position, portname, ":", MAX_NAME_LENGTH);
	 //This checks for the port type!
    if ((length = strcspn (buff + position, ":")) == 0)	//If no port!
    {
        cursector->ports = NULL;
        porttype = 10;
        position++;
    }
    else
        porttype = popint (buffer + position, ":");
	 if (beacon != NULL)
	 {
	 	if (strlen(beacon)!=0)
	 	{
    		if (cursector->beacontext == NULL)
        		cursector->beacontext = (char *) 
						  malloc (sizeof(char)*(strlen (beacon) + 1));
	 		strncpy (cursector->beacontext, beacon, strlen (beacon) + 1);
	 	}
	 }
	 if (nebulae!=NULL)
	 {
		if (strlen(nebulae)!=0)
	 	{
    		if (cursector->nebulae == NULL)
        		cursector->nebulae = (char *) 
						  malloc(sizeof(char)*(strlen (nebulae) + 1));
	  		strncpy (cursector->nebulae, nebulae, strlen (nebulae) + 1);
	 	}
	 }
	 if (portname != NULL)
	 {
    	if (strlen (portname) != 0)
    	{
        if ((curport = (struct port *) malloc (sizeof (struct port))) != NULL)
        {
            curport->name = (char *) 
					malloc(sizeof(char)*(strlen (portname) + 1));
            strncpy (curport->name, portname, strlen (portname) + 1);
            if (porttype != 10)
                curport->type = porttype;
            else
            {
                free (curport->name);
                free (curport);
                curport = NULL;
            }
            cursector->ports = curport;
        }
        else
        {
            printf ("\nUnable to allocate memory");
            return (cursector->number);
        }
		}
	 }
    if ((length = strcspn (buff + position, ":")) == 0)	//If no players
    {
        cursector->players = NULL;	//No players!
        position++;
    }
    else
    {
        len = pos = 0;
        strcpy (tempbuf, "\0");
        popstring (buffer + position, tempbuf, ":", MAX_NAME_LENGTH);
        strncpy (temp, tempbuf, strlen (tempbuf));
        for (counter = 0; counter <= MAX_PLAYERS; counter++)
        {
            if ((curplayer = (struct player *) malloc (sizeof (struct player))) != NULL)
            {
                tempplayer = popint (temp, ",");
                curplayer->name = NULL;
                curplayer->title = NULL;
                curplayer->next = NULL;
                curplayer->pship = NULL;
                psinfo (sockid, tempplayer, curplayer);
                tempplayer = popint (tempbuf, ",");	//For wierd
                if (strncmp (temp, tempbuf, 5) != 0)	//Data corruption
                    strcpy (temp, tempbuf);	//errors
                if (cursector->players == NULL)
                    cursector->players = curplayer;
                else
					 {
					   place=cursector->players;
						while(place->next!=NULL)
						{
							place=place->next;
						}
					 }
					 if (place != NULL)
                	place->next = curplayer;
                curplayer = NULL;
                if (strlen (temp) == 0)	//If we're beyond the length of
                    counter = MAX_PLAYERS + 1;	//the string, then no more loop
            }
            else
            {
                printf ("\nUnable to allocate memory");
                return (cursector->number);
            }
        }
    }
    position = position + 2;	//to get rid of fighter stuff
    if ((length = strcspn (buff + position, ":")) == 0)	//If no planets
    {
        cursector->planets = NULL;	//No planets!
        position++;
    }
    else
    {
        len = pos = 0;
        strcpy (tempbuf, "\0");
        popstring (buffer + position, tempbuf, ":", 200);
        strncpy (temp, tempbuf, strlen (tempbuf));
        for (counter = 0; counter <= MAX_PLANETS; counter++)
        {
            if ((curplanet =
                        (struct planet *) malloc (sizeof (struct planet))) != NULL)
            {
                tempplayer = popint (temp, ",");
                curplanet->name = NULL;
                curplanet->type = NULL;
                curplanet->next = NULL;
                curplanet->name = (char *) malloc (80 * sizeof (char));
                curplanet->type = (char *) malloc (80 * sizeof (char));
                popstring (temp, curplanet->name, ",", MAX_NAME_LENGTH);
                popstring (temp, curplanet->type, ",", MAX_NAME_LENGTH);
                if ((strlen (temp) == 0) || (strcmp (temp, curplanet->type) == 0))	//If we're beyond the length of
                    counter = MAX_PLANETS + 1;	//the string, then no more loop
                /*if (strncmp (temp, tempbuf, 5) != 0)  //Data corruption
                   strcpy (temp, tempbuf);      //errors */
                if (cursector->planets == NULL)
                    cursector->planets = curplanet;
                else
                {
                    pplace->next = curplanet;
                }
                pplace = curplanet;
                if (curplanet->next != NULL)
                    curplanet->next = NULL;
                curplanet = NULL;
            }
            else
            {
                printf ("\nUnable to allocate memory");
                return (cursector->number);
            }
        }
    }
    // To be continued.
    return cursector->number;
}

int printsector (struct sector *cursector)
{
    int len, counter;
    struct player *first = NULL, *place = NULL, *after = NULL;
    struct planet *curplanet = NULL, *pnext = NULL;

    printf ("\n%sSector  %s: %s%d %sin ", KLTGRN, KLTYLW, KLTCYN,
            cursector->number, KGRN);

    printf ("%s", cursector->nebulae);
    free (cursector->nebulae);
    cursector->nebulae = NULL;
	 if (cursector->beacontext != NULL)
	 {
    	if ((len = strlen (cursector->beacontext)) != 0)
    	{
        printf ("\n%sBeacon  %s:%s ", KMAG, KLTYLW, KRED);
        printf ("%s %s", cursector->beacontext, KNRM);
        free(cursector->beacontext);
        cursector->beacontext = NULL;
    	}
	 }
    if (cursector->ports != NULL)
    {
        printf ("\n%sPorts   %s:%s ", KMAG, KLTYLW, KLTCYN);
        printf ("%s%s, %sClass %s%d%s (%s%s)", cursector->ports->name, KLTYLW,
                KMAG, KLTCYN, cursector->ports->type, KMAG,
                porttypes[cursector->ports->type], KMAG);
        if (cursector->ports->type == 9)
            printf (" %s(Stardock)%s", KYLW, KNRM);
        free(cursector->ports->name);
        free(cursector->ports);
    }
    if (cursector->planets != NULL)
    {
        printf ("\n%sPlanets %s:%s ", KMAG, KLTYLW, KGRN);
        curplanet = cursector->planets;
        for (counter = 0; counter <= MAX_PLANETS; counter++)
        {
            if (counter == 0)
            {
                printf ("%s(%s%s%s) %s", KGRN, KLTYLW, cursector->planets->type,
                        KGRN, cursector->planets->name);
            }
            else
            {
                printf ("\n          %s(%s%s%s) %s", KGRN, KLTYLW,
                        curplanet->type, KGRN, curplanet->name);
            }
            if (curplanet->next == NULL)
                counter = MAX_PLANETS + 1;
            else
                pnext = curplanet->next;
            free (curplanet->name);
            free (curplanet->type);
            free (curplanet);
            curplanet = pnext;
        }

    }
    if (cursector->players != NULL)
    {
        printf ("\n%sTraders %s: ", KYLW, KLTYLW);
        first = cursector->players;
        place = first;
        for (counter = 0; counter <= MAX_PLAYERS; counter++)
        {
            if (counter == 0)
            {
                printf ("%s", first->title);
                printf (" %s%s,%s w/ %s%d%s ftrs%s,", first->name, KLTYLW,
                        KGRN, KLTYLW, first->pship->fighters, KGRN, KLTYLW);
            }
            else
            {
                printf ("\n         %s %s%s,%s w/ %s%d%s ftrs%s,", place->title,
                        place->name, KLTYLW, KGRN, KLTYLW,
                        place->pship->fighters, KGRN, KLTYLW);
            }
            printf ("\n           %sin %s%s%s (%s%s)%s",
                    KGRN, KCYN, place->pship->name, KGRN,
                    place->pship->type, KGRN, KNRM);
            if (place->next == NULL)
                counter = MAX_PLAYERS + 1;
            else
                after = place->next;
				fflush(stdout);
            clearplayer (place);
            place = after;
        }
    }
    cursector->players = NULL;
    //All other sector printing stuff goes before this!

    printf ("\n%sWarps to Sector(s) %s:%s  %d", KLTGRN, KLTYLW,
            KLTCYN, cursector->warps[0]);
    for (counter = 1; counter < MAX_WARPS_PER_SECTOR; counter++)
    {
        if (cursector->warps[counter] == 0)
            break;
        printf (" %s- %s%d", KGRN, KLTCYN, cursector->warps[counter]);
    }
    return cursector->number;
}

void clearplayer (struct player *curplayer)
{
	 newfree(curplayer->pship->name);
    newfree(curplayer->pship->type);
    newfree(curplayer->pship);
    newfree(curplayer->name);
    newfree(curplayer->title);
    newfree(curplayer);
    return;
}

void newfree(void *item)
{
	if (item != NULL)
		free(item);
}
void do_ship_upgrade(int sockid, struct player *curplayer)
{
	char *buffer = (char *)malloc(BUFF_SIZE);
	char *temp = (char *)malloc(BUFF_SIZE);
	char choice;
	int price_holds;
	int holds=0;
	int price_figs;
	int figs=0;
	int price_shields;
	int shields=0;
	int stillhere=1;
	int holds_can_buy=0;
	int shields_can_buy=0;
	int figs_can_buy=0;
	int amount=0;
	int type=0;
	int dummy=0;

	while (stillhere)
	{
		getmyinfo(sockid, curplayer);
		*buffer='\0';
		strcpy(buffer, "PORT UPGRADE:7:1:0:");
		sendinfo(sockid, buffer);
		recvinfo(sockid, buffer);
		popstring(buffer, temp, ":", BUFF_SIZE);
		price_holds = popint(temp, ",");
		holds = popint(temp, ",");
		popstring(buffer, temp, ":", BUFF_SIZE);
		price_shields = popint(temp, ",");
		shields = popint(temp, ",");
		popstring(buffer, temp, ":", BUFF_SIZE);
		price_figs = popint(temp, ",");
		figs = popint(temp, ",");
	
		*buffer='\0';
		strcpy(buffer, "PORT UPGRADE:7:1:2:");
		sendinfo(sockid, buffer);
		recvinfo(sockid, buffer);
		popstring(buffer, temp, ":", BUFF_SIZE);
		dummy = popint(temp, ",");
		holds_can_buy = popint(temp, ",");
		popstring(buffer, temp, ":", BUFF_SIZE);
		dummy = popint(temp, ",");
		shields_can_buy = popint(temp, ",");
		popstring(buffer, temp, ":", BUFF_SIZE);
		dummy = popint(temp, ",");
		figs_can_buy = popint(temp, ",");

		
		printf("\n%sYou have %s%d %scredits.",KGRN,KLTCYN,curplayer->credits,KGRN);
		printf("\n%sCommerce report for: %s04:24:54 PM Sat Oct 10, 2015%s    You can buy:", KLTYLW,KLTCYN, KLTYLW);
		printf("\n%sA %sCargo holds      %s:%s       %d %scredits / next hold             %s%d", KGRN, KMAG, KLTYLW, KLTCYN, price_holds, KMAG, KLTCYN, holds_can_buy);
		printf("\n%sB %sFighters         %s:%s       %d %scredits per fighter             %s%d", KGRN, KMAG, KLTYLW, KLTCYN, price_figs, KMAG, KLTCYN, figs_can_buy);
		printf("\n%sC %sShield Points    %s:%s       %d %scredits per point               %s%d", KGRN, KMAG, KLTYLW, KLTCYN, price_shields, KMAG, KLTCYN, shields_can_buy);
		printf("\n");
		printf("\n%sWhich item do you wish to buy? %s(A,B,C,Q):",KMAG,KLTYLW,KNRM);
		scanf("%c", &choice);

		printf("\n");
		switch (choice)
		{
			case 'a':
			case 'A':
				printf("\n%sYou have %s%d%s holds.",KMAG,KLTCYN,curplayer->pship->holds,KMAG);
				printf("\n%sInstalling your next Cargo Hold will cost %s%d%s credits.",KMAG,KLTCYN,price_holds,KMAG);
				printf("\n%sHow many Cargo Holds do you want installed? ",KMAG);
				scanf("%d", &amount);
				type = 4;
				break;
			case 'b':
			case 'B':
				printf("\n%sYou have %s%d%s fighters.",KMAG,KLTCYN,curplayer->pship->fighters,KMAG);
				printf("\n%sHow many L-3B fighters do you want to buy (Max %d) ? ",KMAG,figs_can_buy);
				scanf("%d", &amount);
				type = 6;
				break;
			case 'c':
			case 'C':
				printf("\n%sYou have %s%d%s shields.",KMAG,KLTCYN,curplayer->pship->shields,KMAG);
				printf("\nHow many shield armor points do you want to buy (Max %d) ? ",shields_can_buy);
				scanf("%d", &amount);
				type = 5;
				break;
			case 'q':
			case 'Q':
				stillhere=0;
				break;
			default:
				printf("\nInvalid Selection Please try again");
				break;
		}
		junkline();
		if (stillhere!=0)
		{
			strcpy(buffer, "PORT UPGRADE:");
			addint(buffer, type, ':', BUFF_SIZE);
			//Add how many we're buying
			addint(buffer, amount, ':', BUFF_SIZE);
			//Tell the server that we are buying, not pricing
			addint(buffer, 1, ':', BUFF_SIZE);
			sendinfo(sockid, buffer);
			recvinfo(sockid, buffer);
		}
	}

	free(buffer);
	free(temp);
	return;
}
void doporting (int sockid, struct player *curplayer)
{
    int maxproduct[3];
    int product[3];
    float percentage[3];
    int credits;
    int type;
    int counter;
    int number;
    char name[50];
    char *buffer = (char *) malloc (BUFF_SIZE);
    char status[3][8];
    int holds = 0;
    int playerprice = 0;
    int offered = 0;
    int accepted = 0;
    int xpgained = 0;
    char pnames[3][20] = {"\x1B[1;36mFuel Ore","\x1B[1;36mOrganics","\x1B[1;36mEquipment"};
    int playerproduct[3];
	 int cur_holds= 0 ;    //Number of holds player has of product
    int testholds = 0;		//Number of holds player can afford according
    //To the test price

    playerproduct[0] = curplayer->pship->ore;
    playerproduct[1] = curplayer->pship->organics;
    playerproduct[2] = curplayer->pship->equipment;

    strcpy (buffer, "PORTINFO");
    sendinfo (sockid, buffer);
    *buffer = '\0';
    recvinfo (sockid, buffer);

    number = popint (buffer, ":");
    popstring (buffer, name, ":", BUFF_SIZE);
    maxproduct[0] = popint (buffer, ":");
    maxproduct[1] = popint (buffer, ":");
    maxproduct[2] = popint (buffer, ":");
    product[0] = popint (buffer, ":");
    product[1] = popint (buffer, ":");
    product[2] = popint (buffer, ":");
    credits = popint (buffer, ":");
    type = popint (buffer, ":");
    *buffer = '\0';

    for (counter = 0; counter <= 2; counter++)
    {
        if (portconversion[type][counter] == 'B')
        {
            strcpy (status[counter], "Buying");
            product[counter] = maxproduct[counter] - product[counter];
        }
        else if (portconversion[type][counter] == 'S')
            strcpy (status[counter], "Selling");
        percentage[counter] =
            100.0 * ((float) product[counter] / (float) maxproduct[counter]);
    }

    printf ("\n%s%s<Port>%s", KBBLU, KFWHT, KNRM);
    printf ("\n");
    //
    //Docking...
    //One turn deducted, <number> turns left.
    //
	 if (type==0)
	 {
		free(buffer);
		do_ship_upgrade(sockid, curplayer);
		return;
	 }
				
    printf ("\n%sCommerce report for %s%s%s", KLTYLW, KLTCYN, name, KLTYLW);
    printf ("\n");
    //-=-=-         Docking Log        -=-=-
    //<Ship name> docked <time> ago.
    //
    printf ("\n%s Items     Status\tTrading %% of max OnBoard", KGRN);
    printf ("\n%s -----     ------\t------- -------- -------", KMAG);
    printf ("\n%sFuel Ore   %s%s\t  %s%d\t  %s%d%s%%\t   %s%d", KLTCYN, KGRN,
            status[0], KLTCYN, product[0], KGRN, (int) percentage[0], KLTRED,
            KCYN, curplayer->pship->ore);
    printf ("\n%sOrganics   %s%s\t  %s%d\t  %s%d%s%%\t   %s%d", KLTCYN, KGRN,
            status[1], KLTCYN, product[1], KGRN, (int) percentage[1], KLTRED,
            KCYN, curplayer->pship->organics);
    printf ("\n%sEquipment  %s%s\t  %s%d\t  %s%d%s%%\t   %s%d", KLTCYN, KGRN,
            status[2], KLTCYN, product[2], KGRN, (int) percentage[2], KLTRED,
            KCYN, curplayer->pship->equipment);
    printf ("\n");
    for (counter = 0; counter <= 2; counter++)
    {
        if (product[counter] > playerproduct[counter])
            testholds = playerproduct[counter];
        else
            testholds = product[counter];
        if ((portconversion[type][counter] == 'B')
                && (playerproduct[counter] > 0))
        {
            printf ("\n");
            printf ("\n%sYou have %s%d%s credits and %s%d%s empty cargo holds.",
                    KGRN, KLTCYN, curplayer->credits, KGRN, KLTCYN,
                    curplayer->pship->emptyholds, KGRN);
            printf ("\n");

            printf
            ("\n%sWe are buying up to %s%d%s. You have %s%d%s in your holds.",
             KMAG, KLTYLW, product[counter], KMAG, KLTYLW,
             playerproduct[counter], KMAG);
            printf ("\n%sHow many holds of %s%s do you want to sell [%s%d%s]? ",
                    KMAG, pnames[counter], KMAG, KLTYLW, testholds, KMAG);
            scanf ("%d", &holds);
				if (counter==0)
					cur_holds = curplayer->pship->ore;
				else if (counter == 1)
					cur_holds = curplayer->pship->organics;
				else if (counter == 2)
					cur_holds = curplayer->pship->equipment;
					
            if (holds > 0 && cur_holds == holds)
            {
                printf ("\n%sAgreed, %s%d%s units.", KLTCYN, KLTYLW, holds,
                        KLTCYN);
                *buffer = '\0';
                strcpy (buffer, "PORT TRADE:");
                addint (buffer, counter, ':', BUFF_SIZE);
                addint (buffer, holds, ':', BUFF_SIZE);
                addint (buffer, 0, ':', BUFF_SIZE);
                sendinfo (sockid, buffer);
                *buffer = '\0';
                recvinfo (sockid, buffer);
                offered = popint (buffer, ":");
                accepted = popint (buffer, ":");
                xpgained = popint (buffer, ":");
                do
                {
                    printf ("\n");
                    printf ("\n%sWe'll buy them for %s%d%s credits.", KGRN,
                            KLTYLW, offered, KGRN);
                    printf ("\n%sYour offer [%s%d%s]? ", KMAG, KLTYLW, offered,
                            KMAG);
                    scanf ("%d", &playerprice);
                    *buffer = '\0';
                    strcpy (buffer, "PORT TRADE:");
                    addint (buffer, counter, ':', BUFF_SIZE);
                    addint (buffer, holds, ':', BUFF_SIZE);
                    addint (buffer, playerprice, ':', BUFF_SIZE);
                    sendinfo (sockid, buffer);
                    *buffer = '\0';
                    recvinfo (sockid, buffer);
                    offered = popint (buffer, ":");
                    accepted = popint (buffer, ":");
                    xpgained = popint (buffer, ":");
						  if (accepted == -1)
							accepted = 1;
                }
                while (!accepted);
                if (xpgained > 0)
                    printf
                    ("\n%sFor your great trading you receive %s%d%s experience point(s).",
                     KGRN, KLTYLW, xpgained, KGRN);
                getmyinfo (sockid, curplayer);
            }
        }
    }
    for (counter = 0; counter <= 2; counter++)
    {
        if (curplayer->pship->emptyholds == 0)
            break;
        if ((strncmp (status[counter], "Selling", 7) == 0))
        {
            strcpy (buffer, "PORT TRADE:");
            addint (buffer, counter, ':', BUFF_SIZE);
            addint (buffer, 1, ':', BUFF_SIZE);
            addint (buffer, -1, ':', BUFF_SIZE);	//Get a test price
            sendinfo (sockid, buffer);
            *buffer = '\0';
            recvinfo (sockid, buffer);
            offered = popint (buffer, ":");
            accepted = popint (buffer, ":");
            xpgained = popint (buffer, ":");
            if (curplayer->pship->emptyholds < curplayer->credits / offered)
                testholds = curplayer->pship->emptyholds;
            else
                testholds = curplayer->credits / offered;
            printf ("\n");
            printf ("\n%sYou have %s%d%s credits and %s%d%s empty cargo holds.",
                    KGRN, KLTCYN, curplayer->credits, KGRN, KLTCYN,
                    curplayer->pship->emptyholds, KGRN);
            printf ("\n");
            printf
            ("\n%sWe are selling up to %s%d%s. You have %s%d%s in your holds.",
             KMAG, KLTYLW, product[counter], KMAG, KLTYLW,
             playerproduct[counter], KMAG);
            printf ("\n%sHow many holds of %s%s do you want to buy [%s%d%s]? ",
                    KMAG, pnames[counter], KMAG, KLTYLW, testholds, KMAG);
            scanf ("%d", &holds);
            if (holds > 0 && (curplayer->pship->emptyholds >= holds))
            {
                printf ("\n%sAgreed, %s%d%s units.", KLTCYN, KLTYLW, holds,
                        KLTCYN);
                *buffer = '\0';
                strcpy (buffer, "PORT TRADE:");
                addint (buffer, counter, ':', BUFF_SIZE);
                addint (buffer, holds, ':', BUFF_SIZE);
                addint (buffer, 0, ':', BUFF_SIZE);
                sendinfo (sockid, buffer);
                *buffer = '\0';
                recvinfo (sockid, buffer);
                offered = popint (buffer, ":");
                accepted = popint (buffer, ":");
                xpgained = popint (buffer, ":");
                do
                {
                    printf ("\n");
                    printf ("\n%sWe'll sell them for %s%d%s credits.", KGRN,
                            KLTYLW, offered, KGRN);
                    printf ("\n%sYour offer [%s%d%s]? ", KMAG, KLTYLW, offered,
                            KMAG);
                    scanf ("%d", &playerprice);
                    *buffer = '\0';
                    strcpy (buffer, "PORT TRADE:");
                    addint (buffer, counter, ':', BUFF_SIZE);
                    addint (buffer, holds, ':', BUFF_SIZE);
                    addint (buffer, playerprice, ':', BUFF_SIZE);
                    sendinfo (sockid, buffer);
                    *buffer = '\0';
                    recvinfo (sockid, buffer);
                    offered = popint (buffer, ":");
                    accepted = popint (buffer, ":");
                    xpgained = popint (buffer, ":");
						  if (accepted == -1)
								accepted = 1;
                }
                while (!accepted);
                if (xpgained > 0)
                    printf("\n%sFor your great trading you receive %s%d%s experience point(s).",
                     KGRN, KLTYLW, xpgained, KGRN);
                getmyinfo (sockid, curplayer);
            }
        }
    }
    printf ("\n");
    printf ("\n%sYou have %s%d%s credits and %s%d%s empty cargo holds.", KGRN,
            KLTCYN, curplayer->credits, KGRN, KLTCYN,
            curplayer->pship->emptyholds, KGRN);
    printf ("\n");
	 strcpy(buffer, "PORT QUIT:");
	 sendinfo(sockid, buffer);
	 recvinfo(sockid, buffer);
    curplayer->pship->ported = curplayer->pship->ported + 1;
	 free(buffer);

}

int movesector (char *holder, int sockid, int current, struct sector *cursector)
{
    enum prompts ptype;
    int sector;
    int i;
    char *buff = (char *) malloc (BUFF_SIZE);
    char *temp = (char *) malloc (BUFF_SIZE);
    int pos, len;
    int length;
    int counter, foo = 0;
    char tempsec[5] = "\0";
    int warps[25] =
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0
        };
    char *intptr = (char *) malloc (10);
    char *duplicate = NULL;
    int foobar;
    struct pollfd checkin[1];

    strcpy (buff, "\0");
    strcpy (temp, "\0");
    strcpy (intptr, "\0");
    checkin[0].fd = sockid;
    checkin[0].events = POLLIN;

    for (i = 0; i <= (sizeof (MAXWARPS) + 2); i++)
    {
        if (isdigit (*(holder + i) == 0))
            *(holder + i) = '\0';
    }
    sector = strtoul (holder, NULL, 10);
    holder = NULL;
    if ((sector == 0) || (sector > MAXWARPS))
    {
        printf ("\nInvalid Sector Number");
        return current;
    }
    if (sector == current)
    {
        printf ("\nMoving from %d to %d is rather pointless.", current, sector);
        free (buff);
        free (temp);
        free (intptr);
        buff = NULL;
        temp = NULL;
        intptr = NULL;
        return sector;
    }
    printf ("\n%s%s<Move>%s", KBBLU, KFWHT, KNRM);
    printf ("\n%sWarping to Sector %s%d\n", KMAG, KYLW, sector);
    sprintf (intptr, "%d", sector);
    sendinfo (sockid, intptr);
    recvinfo (sockid, buff);	//Put a check error into here
    if (strncmp (buff, ":", 1) != 0)
    {
        sendinfo (sockid, "UPDATE");
        recvinfo (sockid, buff);
        duplicate = strdup (buff);
        if (strncmp (duplicate, "OK: Still in Transit", 20) != 0)
            getmessages (duplicate);
        free (duplicate);
        while (strncmp (buff, "OK", 2) == 0)
        {
            usleep (500000);	//Wait half a second
            sendinfo (sockid, "UPDATE");
            recvinfo (sockid, buff);
            duplicate = strdup (buff);
            if (strncmp (duplicate, "OK: Still in Transit", 20) != 0)
                getmessages (duplicate);
            free (duplicate);

            fflush (stdout);
        }
    }
    //recvinfo(sockid, buff);
    len = 0;
    length = 0;
    pos = 1;			//First part of move is a delimiter
    if ((len = strcspn (buff + pos, ":")) == 0)
    {
        free (buff);
        free (temp);
        free (intptr);
        buff = NULL;
        temp = NULL;
        intptr = NULL;
        return -1;		//No sector
    }
    strncpy (temp, buff + pos, len);
    if (strlen (temp) != len)
        *(temp + len) = '\0';
    if ((length = strcspn (temp, ",")) != len)	//In case we're not adjacent
    {
        pos = length = 0;
        for (counter = 0; counter <= 25; counter++)
        {
            if ((length = strcspn (temp + pos, ",")) == 0)
            {
                free (buff);
                free (temp);
                free (intptr);
                buff = NULL;
                temp = NULL;
                intptr = NULL;
                return -1;
            }
            strncpy (tempsec, temp + pos, length);
            tempsec[length] = '\0';
            warps[counter] = strtoul (tempsec, NULL, 10);
            pos = pos + length + 1;
            if (pos >= len)
            {
                foo = counter;
                counter = 26;
            }
        }

        printf ("\n%sThat Warp lane is not adjacent.%s", KGRN, KNRM);
        printf ("\n\n%sComputed.%s", KBLU, KNRM);
        printf
        ("\n\n%sThe Shortest path is %s( %d hops, %d turns)%s from sector %s%d%s to sector %s%d%s is%s:%s",
         KGRN, KMAG, foo, (foo * 3), KGRN, KYLW, current, KGRN, KYLW, sector,
         KGRN, KYLW, KNRM);
        printf ("\n%s%d", KLTRED, warps[0]);
        for (counter = 1; counter <= 25; counter++)
        {
            if (warps[counter] == 0)
            {
                break;
            }
            printf (" %s> %s%d", KYLW, KGRN, warps[counter]);
        }
        printf ("\n\n");
        ptype = autopilot;
        if (getyes (prompttype (ptype, 0, sockid)))
        {
            printf ("\n%s%s<Auto Pilot Engaging>%s", KBBLU, KFWHT, KNRM);
            for (counter = 1; counter <= foo; counter++)
            {
                printf ("\n\n%sAuto Warping to Sector %s%d\n", KYLW,
                        KLTYLW, warps[counter]);
                sprintf (intptr, "%d", warps[counter]);
                sendinfo (sockid, intptr);
                recvinfo (sockid, buff);
                sendinfo (sockid, "UPDATE");
                recvinfo (sockid, buff);
                duplicate = strdup (buff);
                if (strncmp (duplicate, "OK: Still in Transit", 20) != 0)
                    getmessages (duplicate);
                free (duplicate);
                while (strncmp (buff, "OK", 2) == 0)
                {
                    //la la la do nothing.. for now...
                    //Check out the realtime thread here via polling
                    usleep (500000);
                    sendinfo (sockid, "UPDATE");
                    recvinfo (sockid, buff);
                    duplicate = strdup (buff);
                    if (strncmp (duplicate, "OK: Still in Transit", 20) != 0)
                        getmessages (duplicate);
                    free (duplicate);
                    fflush (stdout);
                }
                //recvinfo(sockid, buff);
                getsectorinfo (sockid, cursector);
                printsector (cursector);
            }
            printf ("\n\n%sArriving Sector :%s%s%d%s Autopilot disengaging.",
                    KCYN, KBBLU, KFWHT, warps[counter - 1], KCYN);
            foobar = warps[counter - 1];
        }
        else
            foobar = current;
    }
    else				//Otherwise we are and just move there
    {
        getsectorinfo (sockid, cursector);
        printsector (cursector);
        foobar = sector;
    }
    free (buff);
    free (temp);
    free (intptr);
    buff = NULL;
    temp = NULL;
    intptr = NULL;
    return foobar;
}


char *prompttype (enum prompts type, int sector_or_porttype, int sockid)
{
    int hour = 0;
    int min = 0;
    int sec = 0;
    int test = 0;
    char *input = NULL;
    char *duplicate = NULL;
    char *buffer = (char *) malloc (BUFF_SIZE);

    int forever = 1;
    struct pollfd check[1];

    check[0].fd = 0;		//Check out STDIN
    check[0].events = POLLIN | POLLPRI;
    while (forever)
    {
        switch (type)
        {
        case command:
            gettimeofday (&end, 0);
            sec = end.tv_sec - t.tv_sec;
            if ((end.tv_sec - t.tv_sec) >= 60)
            {
                test = sec / 60;
                sec = sec % 60;
                min = min + test;
                if (min >= 60)
                    ;
                {
                    test = min / 60;
                    min = min % 60;
                    hour = hour + test;
                }
            }

            printf("\n\n%sCommand [%sTL%s=%s%d:%d:%d%s]%s:%s[%s%d%s] (%s?=Help%s)? :",
             KMAG, KLTYLW, KYLW, KLTYLW, hour, min, sec, KMAG, KBLD, KMAG,
             KLTCYN, sector_or_porttype, KMAG, KLTYLW, KMAG);
            //Prints out the Main command line
            printf ("%s ", KNRM);	//Gets rid of any extra colors.
            break;
        case autopilot:
            printf ("\n%sEngage the Autopilot? %s(Y/N/Single step/Express) %s",
                    KMAG, KYLW, KMAG);
            break;
        case quit:
            printf ("\n%sConfirmed? (Y/N)? ", KMAG);
            break;
        case pt_port:
            printf ("%s\n", KNRM);
				//Attack goes here
				if (sector_or_porttype==9)
					printf("\n%s<%sS%s>%s Land on the %sStardock", KMAG, KGRN, KMAG, KGRN, KYLW);
            printf("\n%s<%sT%s>%s Trade at this Port", KMAG, KGRN, KMAG, KGRN);
            printf("\n%s<%sQ%s>%s Quit", KMAG, KGRN, KMAG, KGRN);
            printf("\n\n%sEnter your choice %s[T]%s ?", KMAG, KLTYLW, KMAG);
            break;
        case move:
            printf ("\n%sTo which Sector ?", KMAG);
            break;
        default:
            break;

        }
        fflush (stdout);

        while ((poll (check, 1, 1) != 1))	//Dont forget to put infinite in
        {
            // Check the real time stuff here!
            // And then print out real time stuff!!!
            sendinfo (sockid, "UPDATE");
            recvinfo (sockid, buffer);
            duplicate = strdup (buffer);
            if (strncmp (duplicate, "OK", 2) == 0)
            {
                printf ("\n");
                getmessages (duplicate);
                free (duplicate);
                forever = 1;
                break;
            }
            forever = 0;
            //free(duplicate);
            fflush (stdout);
        }
    }
    input = malloc (10 * sizeof (char));
    read (0, input, 10);		//Get 10 characters from stdin!
    return input;
}

int
dologin (int sockid)
{
    char *buffer = NULL;		//To store stuff
    char *stuff = NULL;
    char *password;		//user password
    char ch = '\0';
    int loop = 1, counter;
    int i = 0, j = 0;
    char blorp[50], garbage[50];
    char feefo[BUFF_SIZE] = "\0";
    int newplayer = 0;
    buffer = blorp;
    stuff = garbage;

    for (; loop;)
    {
        printf ("\nPlease enter your username and password");
        printf ("\nOr type NEW to make a new user");
        printf ("\nUser: ");
        for (counter = 0; (ch = getchar ()) != '\n'; counter++)
        {
            if (isspace (ch) == 0)
                *(buffer + counter) = ch;
        }
        *(buffer + counter) = '\0';
        if (strncmp (buffer, "NEW", 3) == 0)
        {
            strcpy (buffer, "NEW ");	//Just to make sure everything is good
            newplayer = 1;
            printf
            ("\nNote: Please save after entering the game so the server can save you correctly\n");
            printf ("\nPlease enter your desired User Name and Password");
            printf ("\nUser: ");
            for (counter = 4; (ch = getchar ()) != '\n'; counter++)
            {
                if (isspace (ch) == 0)
                    *(buffer + counter) = ch;
            }
            *(buffer + counter++) = ':';
            *(buffer + counter) = '\0';
            printf ("\nPassword: ");
            fflush (stdout);
            password = get_invis_password ();
            counter += strlen (password);
            strcat (buffer, password);
            *(buffer + counter) = ':';
            printf ("%s\nPlease enter a ship name that you would like", KNRM);
            printf ("\nShip Name: ");
            for (counter++; (ch = getchar ()) != '\n'; counter++)
            {
                if ((isalnum (ch) != 0) || (isspace (ch) != 0))
                    *(buffer + counter) = ch;
            }
            *(buffer + counter) = ':';
            *(buffer + counter + 1) = '\0';
        }
        else
        {
            strcpy (stuff, "USER ");
            *(buffer + counter) = ':';
            *(buffer + counter + 1) = '\0';
            printf ("Password: ");
            fflush (stdout);
            password = get_invis_password ();
            counter += strlen (password) + 1;
            strcat (buffer, password);
            *(buffer + counter) = ':';
            // I'm not sure If I need to change the following j to
            //MAX_WARPS_PER_SECTOR - 1 or not.

            j = 5;		//element 5 of stuff
            for (i = 0; i <= counter; i++)
            {
                *(stuff + j) = *(buffer + i);
                j++;
            }
            *(stuff + j) = '\0';
            strcpy (buffer, stuff);
        }
        i = 0;
        j = 0;
        for (i = 0; i <= strlen (buffer); i++)
        {
            if (*(buffer + i) == ':')
                j++;
        }
        if ((j > 2) && newplayer == 0)
            printf ("%s\nInvalid login name and or password please try again.",
                    KNRM);
        else if ((j == 2) || ((j == 3) && (newplayer == 1)))
        {
            sendinfo (sockid, buffer);
            stuff = feefo;
            recvinfo (sockid, stuff);
            if (strncmp (stuff, "BAD", 3) == 0)	//If bad username and password
            {
                printf ("%s\nIncorrect Username and or Password", KNRM);
                loop = 1;		//back to the beginning
            }
            else
            {
                loop = 0;		//otherwise get outta here
                printf ("%s\n", KNRM);
            }
        }
        else
            perror ("dologin(): Missing delimiters -");
    }
    return (1);
}

int
getyes (char *answer)
{
    if (*(answer + 0) == 'y' || *(answer + 0) == 'Y')
    {
        return 1;
    }
    return 0;
}

/* This function deals with real time messages
   * Currently Only dealing with moving
 */
void
getmessages (char *buffer)
{
    char temp[10];
    char name[50];
    char message[BUFF_SIZE];
    int direction = -255;

    if (strncmp (buffer, "OK:", 3) != 0)
        return;
    //fprintf(stderr, "\ngetmessages: Buffer is '%s'", buffer);
    popstring (buffer, temp, ":", BUFF_SIZE);
    popstring (buffer, name, ":", BUFF_SIZE);
    direction = popint (buffer, ":");
    popstring (buffer, message, ":", BUFF_SIZE);

    switch (direction)
    {
    case 1:
        printf ("\n%s%s%s warps into the sector.", KLTCYN, name, KGRN);
        break;
    case -1:
        printf ("\n%s%s%s warps out of the sector.", KLTCYN, name, KGRN);
        break;
    case 0:
        printf
        ("\n\n%sIncoming transmission from %s%s%s on Federation comm-link:",
         KGRN, KLTCYN, name, KGRN);
        printf ("\n%s%s", KLTYLW, message);
        printf ("\n");
        break;
    case 2:
        printf ("\n%s%s%s enters the game.", KLTCYN, name, KGRN);
        break;
    case -2:
        printf ("\n%s%s%s leaves the game.", KLTCYN, name, KGRN);
        break;
	 case 3:
		  printf("\n%s%s%s lands at the port.", KLTCYN, name, KGRN);
		  break;
	 case -3:
		  printf("\n%s%s%s leaves the port.", KLTCYN, name, KGRN);
		  break;
	 case 4:
		  printf("\n%s%s%s lands on the Stardock.", KLTCYN, name, KGRN);
		  break;
	 case -4:
		  printf("\n%s%s%s blasts off the Stardock.", KLTCYN, name, KGRN);
		  break;
    default:
        break;
    }
}
int
init_nowait_io ()
{
    tcgetattr (0, &orig);
    new = orig;
    new.c_lflag &= ~ICANON;
    new.c_lflag &= ~ECHO;
    new.c_lflag &= ~ISIG;
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    return tcsetattr (0, TCSANOW, &new);
}


int
done_nowait_io (int status)
{
    tcsetattr (0, TCSANOW, &orig);
    return (status);
}


int
kbhit ()
{
    char ch;
    int nread;

    if (peek != -1)
        return 1;
    new.c_cc[VMIN] = 0;
    tcsetattr (0, TCSANOW, &new);
    nread = read (0, &ch, 1);
    new.c_cc[VMIN] = 1;
    tcsetattr (0, TCSANOW, &new);

    if (nread == 1)
    {
        peek = ch;
        return 1;
    }

    return 0;
}

int
readch ()
{
    char ch;

    if (peek != -1)
    {
        ch = peek;
        peek = -1;
        return ch;
    }

    read (0, &ch, 1);
    return ch;
}

//this function returns a password of up to
//10 chars.  if someone has a longer pw it
//truncates it to a shorter pw.
char *
get_invis_password (void)
{
    char *pw_buffer, nextchar;
    int doneyet = 0, i = 0;
    pw_buffer = (char *) malloc (11 * sizeof (char));
    init_nowait_io ();
    while (!doneyet)
    {
        while (!kbhit ())
            ;	// wait for user to say something.
        nextchar = readch ();
        if (nextchar == '\n')
            doneyet = 1;
        else
            pw_buffer[i++] = nextchar;
    }
    pw_buffer[i] = '\0';
    done_nowait_io (0);
    return pw_buffer;
}
