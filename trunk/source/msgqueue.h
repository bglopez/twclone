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

#ifndef MSG_QUEUE_H
#define MSG_QUEUE_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

enum commandtype
{
  ct_quit,
  ct_describe,
  ct_move,
  ct_login,
  ct_newplayer,
  ct_logout,
  ct_playerinfo,
  ct_shipinfo,
  ct_port,
  ct_info,
  ct_portinfo,
  ct_update
};
  
struct msgbuffer
{
  long mtype;
  char buffer[BUFF_SIZE];
  long senderid;
};

struct msgcommand
{
  long mtype;
  enum commandtype command;
  enum porttype pcommand;
  char name[MAX_NAME_LENGTH + 1];
  char passwd[MAX_NAME_LENGTH + 1];
  char buffer[30];
  int playernum;
  int to;
  long senderid;
};

int init_msgqueue();
long getmsg(int msgid, char *buffer, long mtype);
void sendmsg(int msgid, char *buffer, long mtype);
void senddata(int msgid, struct msgcommand *data, long mtype);
long getdata(int msgid, struct msgcommand *data, long mtype);

#endif
