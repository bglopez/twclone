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
  ct_update,
  ct_fedcomm,
  ct_online,
  ct_stardock,
  ct_land,
  ct_scan,
  ct_beacon,
  ct_tow,
  ct_listnav,
  ct_transporter,
  ct_genesis,
  ct_jettison,
  ct_interdict,
  ct_attack,
  ct_etherprobe,
  ct_fighters,
  ct_listfighters,
  ct_mines,
  ct_listmines,
  ct_portconstruction,
  ct_setnavs,
  ct_gamestatus,
  ct_hail,
  ct_subspace,
  ct_listwarps,
  ct_cim,
  ct_avoid,
  ct_listavoids,
  ct_selfdestruct,
  ct_listsettings,
  ct_updatesettings,
  ct_dailyannouncement,
  ct_firephoton,
  ct_readmail,
  ct_sendmail,
  ct_shiptime,
  ct_usedisruptor,
  ct_dailylog,
  ct_alienranks,
  ct_listplayers,
  ct_listplanets,
  ct_listships,
  ct_listcorps,
  ct_joincorp,
  ct_makecorp,
  ct_credittransfer,
  ct_fightertransfer,
  ct_minetransfer,
  ct_shieldtransfer,
  ct_quitcorp,
  ct_listcorpplanets,
  ct_showassets,
  ct_corpmemo,
  ct_dropmember,
  ct_corppassword,
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

int init_msgqueue ();
long getmsg (int msgid, char *buffer, long mtype);
void sendmsg (int msgid, char *buffer, long mtype);
void senddata (int msgid, struct msgcommand *data, long mtype);
long getdata (int msgid, struct msgcommand *data, long mtype);

#endif
