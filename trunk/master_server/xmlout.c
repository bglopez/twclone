/*
** Copyright (c) 2002 Rick Dearman <rick@ricken.demon.co.uk>
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** $Author: eryndil $
** $Date: 2002-07-17 14:24:36 $
** $RCSfile: xmlout.c,v $
** $Revision: 1.1.1.1 $
** $State: Exp $
**
** Description: This program is used to output XML data.
**
*/

#include <stdio.h>
#include <nspr.h>
#include <string.h>
#include "server.h"

extern struct gamenode *nodeOne;


void
outputXML (f)
{
  struct gamenode *cgame;

  fprintf ((FILE *) f, "<?xml version=\"1.0\"?>\n\n");
  fprintf ((PRFileDesc *) f, "<SERVERLIST>\n");
  for (cgame = nodeOne; cgame != NULL; cgame = cgame->next)
    {
      fprintf ((FILE *) f, "<SERVER url=\"%s\" host=\"%s\">\n", cgame->url,
	       cgame->host);
      fprintf ((FILE *) f, "<GAME version=\"%f\">\n", cgame->ver);
      fprintf ((FILE *) f, "\t<NAME>%s</NAME>\n", cgame->gamename);
      fprintf ((FILE *) f, "\t<AGE>%d</AGE>\n", cgame->age_of_game);

      fprintf ((FILE *) f, "\t<CORPS>%d</CORPS>\n", cgame->corps_in_game);
      fprintf ((FILE *) f,
	       "\t<SHIPS initialholds=\"%d\" initialcredits=\"%d\" initialfighters=\"%d\">%d</SHIPS>\n",
	       cgame->initial_holds, cgame->initial_credits,
	       cgame->initial_fighters, cgame->ships_in_game);
      fprintf ((FILE *) f, "\t<FIGHTERS >%d</FIGHTERS>\n",
	       cgame->fighters_in_game);
      fprintf ((FILE *) f, "\t<MINES>%d</MINES>\n", cgame->mines_in_game);
      fprintf ((FILE *) f, "\t<TURNSPERDAY>%d</TURNSPERDAY>\n",
	       cgame->turns_per_day);
      fprintf ((FILE *) f, "\t<GAMETYPE>%d</GAMETYPE>\n", cgame->gametype);
      fprintf ((FILE *) f, "\t<PLANETARYTRADE>%d</PLANETARYTRADE>\n",
	       cgame->percent_planetary_trade);
      fprintf ((FILE *) f, "\t<STEALFROMBUYPORT>%d</STEALFROMBUYPORT>\n",
	       cgame->steal_from_buy_port);
      fprintf ((FILE *) f, "\t<BUSTS clear_busts=\"%d\">%d</BUSTS>\n",
	       cgame->clear_busts, cgame->last_bust_clear);
      fprintf ((FILE *) f, "\t<MULTIPLEPHOTON>%d</MULTIPLEPHOTON>\n",
	       cgame->multiple_photon_fire);
      fprintf ((FILE *) f, "\t<SECTORS>%d</SECTORS>\n",
	       cgame->sectors_in_game);
      fprintf ((FILE *) f, "\t<SHIPDELAY>%d</SHIPDELAY>\n",
	       cgame->ship_delay);
      fprintf ((FILE *) f, "\t<DISPLAYSTARDOCK>%d</DISPLAYSTARDOCK>\n",
	       cgame->display_stardock);
      fprintf ((FILE *) f, "\t<STARWITHPLANET>%d</STARWITHPLANET>\n",
	       cgame->start_with_planet);
      fprintf ((FILE *) f, "\t<LOCALDISPLAY>%d</LOCALDISPLAY>\n",
	       cgame->local_display_on);
      fprintf ((FILE *) f, "\t<CLASSICFERRENGI>%d</CLASSICFERRENGI>\n",
	       cgame->classic_ferrengi);
      fprintf ((FILE *) f, "\t<MAXREGEN>%d</MAXREGEN>\n",
	       cgame->max_regen_per_visit);
      fprintf ((FILE *) f, "\t<ROBSTEALDELAY>%d</ROBSTEALDELAY>\n",
	       cgame->rob_steal_delay);
      fprintf ((FILE *) f,
	       "\t<INVINCIBLEFERRENGAL>%d</INVINCIBLEFERRENGAL>\n",
	       cgame->invincible_ferrengal);
      fprintf ((FILE *) f, "\t<TOURNAMENT>%d</TOURNAMENT>\n",
	       cgame->tournament_mode);

      fprintf ((FILE *) f,
	       "\t<PLAYERS maxplayers=\"%d\">\n\t\t<INGAME>%d</INGAME>\n\t\t<PERCENTGOOD>%d</PERCENTGOOD>\n\t</PLAYERS>\n",
	       cgame->maxplayers, cgame->players_in_game,
	       cgame->percent_good_players);
      fprintf ((FILE *) f,
	       "\t<PORTS maxports=\"%d\">\n\t\t<INGAME>%d</INGAME>\n\t\t<VALUE>%d</VALUE>\n\t</PORTS>\n",
	       cgame->maxports, cgame->ports_in_game, cgame->value_of_ports);
      fprintf ((FILE *) f,
	       "\t<PLANETS maxplanets=\"%d\">\n\t\t<INGAME>%d</INGAME>\n\t\t<PERCENTWITHCID>%d</PERCENTWITHCID>\n\t</PLANETS>\n",
	       cgame->maxplanets, cgame->planets_in_game,
	       cgame->percent_planets_with_cid);
      fprintf ((FILE *) f, "</GAME>\n");
      fprintf ((FILE *) f, "</SERVER>\n");
    }
  fprintf ((FILE *) f, "</SERVERLIST>\n");
}
