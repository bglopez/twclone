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
** $RCSfile: server.h,v $
** $Revision: 1.1.1.1 $
** $State: Exp $
**
** Description: This file contains global variables for the 
**              serverlauncher program.
**
*/

struct serverdesc
{
  int listen_port;
  char *log_file;
}
thisserver;


struct gamenode
{
  struct gamenode *next;
  float ver;
  char *host;
  char *url;
  char *gamename;
  int age_of_game;
  int maxplayers;
  int maxports;
  int maxplanets;
  int players_in_game;
  int ports_in_game;
  int planets_in_game;
  int value_of_ports;
  int percent_good_players;
  int percent_planets_with_cid;
  int corps_in_game;
  int ships_in_game;
  int fighters_in_game;
  int mines_in_game;
  int gametype;
  int turns_per_day;
  int percent_planetary_trade;
  int steal_from_buy_port;
  int initial_fighters;
  int initial_holds;
  int initial_credits;
  int clear_busts;
  int last_bust_clear;
  int multiple_photon_fire;
  int sectors_in_game;
  int display_stardock;
  int ship_delay;
  int start_with_planet;
  int local_display_on;
  int classic_ferrengi;
  int max_regen_per_visit;
  int rob_steal_delay;
  int invincible_ferrengal;
  int tournament_mode;
};
