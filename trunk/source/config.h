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

#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"

struct config
{
  int turnsperday;		//In this order in config.data
  int maxwarps;
  int startingcredits;
  int startingfighters;
  int startingholds;
  int processinterval;		//How often stuff is processed
  int autosave;			//Save every some odd minutes
  /*
  int MAX_PLAYERS;
  int MAX_SHIPS;
  int MAX_PORTS;
  int MAX_PLANETS;
  int MAX_TOTAL_PLANETS;
  int MAX_SAFE_PLANETS;
  int MAX_CITADEL_LEVEL;
  int NUMBER_OF_PLANET_TYPES;
  int MAX_SHIP_NAME_LENGTH;
  int SHIP_TYPE_COUNT;*/
};

int init_config (char *filename);

#endif
