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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "parse.h"

extern struct config *configdata;

int init_config(char *filename)
{
   FILE *configfile;
   char buffer[BUFF_SIZE];
   
   if ((configdata=(struct config *)malloc(sizeof(struct config)))==NULL)
   {
      fprintf(stderr, "Unable to allocate enough memory for configuration data\n");
      exit(-1);
   }
	   
   configfile = fopen(filename, "r");
   fgets(buffer, BUFF_SIZE, configfile);
   configdata->turnsperday = popint(buffer, ":");
   configdata->maxwarps = popint(buffer, ":");
   configdata->startingcredits = popint(buffer, ":");
   configdata->startingfighters = popint(buffer, ":");
   configdata->startingholds = popint(buffer, ":");
   configdata->processinterval = popint(buffer, ":");
   configdata->autosave = popint(buffer, ":");
return(1);
}
