# -*- makefile -*-
# $Id: Makefile.am,v 1.1 2002-06-11 21:03:18 eryndil Exp $
# Copyright (C) 2002, Nathan Paul Simons <npsimons@hardcorehackers.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or 
# (at your option) any later version.
#   
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
# GNU General Public License for more details.
#   
# You should have received a copy of the GNU General Public License 
# along with this program; if not, write to the Free Software 
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
#
# Alternatively, the GPL can be found at 
# http://www.gnu.org/copyleft/gpl.html
#
# Standard automake Makefile.am template.

bin_PROGRAMS = server test_server test_client client bigbang

server_SOURCES = server.c \
	common.c \
	universe.c \
	msgqueue.c \
	player_interaction.c \
	sysop_interaction.c \
	hashtable.c \
	parse.c \
	serveractions.c \
	shipinfo.c \
	planet.c \
	config.c \
	maint.c

client_SOURCES = client.c \
	common.c \
	parse.c

bigbang_SOURCES = bigbang.c \
	namegen.c \
	planet.c \
	parse.c \
	config.c
	
test_server_SOURCES = test_server.c \
	common.c

test_client_SOURCES = test_client.c \
	common.c

