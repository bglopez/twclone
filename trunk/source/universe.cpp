/*
 * universe.cpp
 *
 *  Created on: Dec 4, 2009
 *      Author: Ryan
 */

#include "universe.h"
#include <iostream>
#include <string>

Entity::Entity()
{
	// TODO Auto-generated constructor stub

}

Entity::~Entity()
{
	// TODO Auto-generated destructor stub
}

Port::~Port(char *filename)
{
    FILE *portfile;
    int counter;			//Counter and other general usage
    char buffer[BUFF_SIZE];
    char name[MAX_NAME_LENGTH];
    class Port *curport;

	 ports = (struct Port **)
			malloc(sizeof(struct Port *)*configdata->max_ports);
    for (counter = 0; counter <= configdata->max_ports; counter++)
        ports[counter] = NULL;

    portfile = fopen (filename, "r");
    if (portfile == NULL)
    {
        fprintf (stderr,
                 "\ninit_portinfo: No port file! Please rerun bigbang!");
        exit (-1);
    }
    while (1)
    {
        buffer[0] = '\0';
        fgets (buffer, BUFF_SIZE, portfile);
        if (strlen (buffer) == 0)
            break;
        counter = popint (buffer, ":");
        popstring (buffer, name, ":", MAX_NAME_LENGTH);
        if ((curport =
                    (struct Port *) insert (name, port, symbols, HASH_LENGTH)) == NULL)
        {
            fprintf (stderr, "init_portinfo: duplicate portname '%s'\n", name);
            exit (-1);
        }
        curport->number = counter;
        curport->sector = popint (buffer, ":");
        curport->maxproduct[0] = popint (buffer, ":");	//MaxOre
        curport->maxproduct[1] = popint (buffer, ":");	//MaxOrganics
        curport->maxproduct[2] = popint (buffer, ":");	//MaxEquipment
        curport->product[0] = popint (buffer, ":");	//Current Ore
        curport->product[1] = popint (buffer, ":");	//Current Organics
        curport->product[2] = popint (buffer, ":");	//Current Equipment
        curport->credits = popint (buffer, ":");
        curport->type = popint (buffer, ":");
        curport->invisible = popint (buffer, ":");
        curport->name = (char *) malloc (strlen (name) + 1);
        strcpy (curport->name, name);

        if (ports[counter - 1] != NULL)
        {
            fprintf (stderr,
                     "Port(char*): Duplicate port numbers, exiting...\n");
            exit (-1);
        }
        ports[counter - 1] = curport;
        sectors[curport->sector - 1]->portptr = curport;
    }
    fclose (portfile);

}
