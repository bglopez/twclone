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

#ifndef PLAYER_INTERACTION_H
#define PLAYER_INTERACTION_H

#include <signal.h>

#undef max
#define max(x,y) ((x) > (y) ? (x) : (y))
struct connectinfo
{
  int sockid;
  int msgidin;
  int msgidout;
};

struct sockinfo
{
	int sockid;
	char *address;
	char *name;
	struct sockinfo *next;
};
		  
void add_sock(int sockid, char *address);
int del_sock(struct sockinfo *deleteme);
void handle_sockets(int sockid, int msgidin, int msgidout);
void handle_player (struct sockinfo *playersock, int msgidin, int msgidout);
int catchpipes(char *inbuff);
void *makeplayerthreads (void *threadinfo);

#endif
