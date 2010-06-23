/*
 * universe.cpp
 *
 *  Created on: Dec 4, 2009
 *      Author: Ryan
 */


#include <iostream>
#include <string>
#include "shipinfo.h"
#include "parse.h"
#include "hashtable.h"
#include "universe.h"
#include "common.h"
#include "planet.h"
#include "baseconfig.h"

extern Port **ports;

Entity::Entity();

Entity::~Entity()
{
	if (name!=NULL)
		delete name;
}

virtual Entity::Entity(const char* filename);

Port::Port(const char* filename)
{
    FILE *datafile;
    int counter=0;			//Counter and other general usage
    char buffer[BUFF_SIZE];
    bool done = false;

    datafile = fopen (filename, "r");
    if (datafile == NULL)
    {
        fprintf (stderr,
                 "\nNo '%s' file! Please rerun bigbang!", (*filename));
        exit (-1);
    }
    while (!done)
    {
        buffer[0] = '\0';
        fgets (buffer, BUFF_SIZE, datafile);
        if (strlen (buffer) == 0)
            done = true;
        else
        {
        	char tempname[MAX_NAME_LENGTH];

			counter = popint (buffer, ":");
			popstring (buffer, tempname, ":", MAX_NAME_LENGTH);
			if ((curport =
						(Port *) insert (tempname, port, symbols, HASH_LENGTH)) == NULL)
			{
				fprintf (stderr, "init_portinfo: duplicate portname '%s'\n", tempname);
				exit (-1);
			}
			number = counter;
			sector = popint (buffer, ":");
			maxproduct[0] = popint (buffer, ":");	//MaxOre
			maxproduct[1] = popint (buffer, ":");	//MaxOrganics
			maxproduct[2] = popint (buffer, ":");	//MaxEquipment
			product[0] = popint (buffer, ":");	//Current Ore
			product[1] = popint (buffer, ":");	//Current Organics
			product[2] = popint (buffer, ":");	//Current Equipment
			credits = popint (buffer, ":");
			type = popint (buffer, ":");
			invisible = popint (buffer, ":");
			name = new char[strlen(tempname)+1];
			strcpy (name, tempname);

			if (ports[counter - 1] != NULL)
			{
				fprintf (stderr,
						 "Port(char*): Duplicate port numbers, exiting...\n");
				exit (-1);
			}
        }
    }
    fclose (datafile);
}

Port::Port();
Port::~Port();

