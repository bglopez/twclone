#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "planet.h"
#include "parse.h"
#include "config.h"

extern struct config *configdata;
/*
 *	init_planets(filename, secarray)
 *	loads planet info from file.  returns number
 *	of planets in the universe when done
 */

void saveplanets(char *filename)
{
	FILE *planetfile;
	int loop;
	int index=0;
	char *treasury = (char *)malloc(sizeof(char)*BUFF_SIZE);
	char *stufftosave = (char *)malloc(sizeof(char)*BUFF_SIZE);

	planetfile = fopen(planetfile, "w");
	
	for (index=0; index<configdata->max_total_planets; index++)
	{
		if (planets[index] != NULL)
		{
		strcpy(stufftosave, "\0");
		strcpy(treasury, "\0");
		addint(stufftosave, planets[index]->num, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->sector, ':', BUFF_SIZE);
		addstring(stufftosave, planets[index]->name, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->type, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->owner, ':', BUFF_SIZE);
		addstring(stufftosave, planets[index]->creator, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->fuelColonist, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->organicsColonist, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->equipmentColonist, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->fuel, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->organics, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->equipment, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->fighters, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->citdl->level, ':', BUFF_SIZE);
		sprintf(treasury, "%ld", planets[index]->citdl->treasury);
		addstring(stufftosave, treasury, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->citdl->militaryReactionLevel
							 , ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->citdl->qCannonAtmosphere, ':'
							 , BUFF_SIZE);
		addint(stufftosave, planets[index]->citdl->qCannonSector, ':', BUFF_SIZE);
		addint(stufftosave, planets[index]->citdl->planetaryShields, ':'
							 , BUFF_SIZE);
		addint(stufftosave, planets[index]->citdl->transporterlvl, ':'
							 , BUFF_SIZE);
		addint(stufftosave, planets[index]->citdl->interdictor, ':', BUFF_SIZE);
		for (loop=0; loop< 399 - strlen(stufftosave); loop++)
			strcat(stufftosave, " ");
		strcat(stufftosave, "\n");
		fprintf(planetfile, stufftosave);
		}
	}
	free(stufftosave);
	free(treasury);
}
int init_planets (char *filename, struct sector **secarray)
{
    FILE *planetfile;
    int i, p_num, p_sec, p_type, p_owner;
    char *p_name, dummy[3], buffer[BUFF_SIZE], *temp;
    int count = 0;
	 int done=0;

    p_name = (char *) malloc (sizeof (char) * (MAX_NAME_LENGTH + 1));
	 temp = (char *)malloc(sizeof(char)*30);

	 planets = (struct planet **)
			malloc(sizeof(struct planet *)*configdata->max_total_planets);
    for (i = 0; i < configdata->max_total_planets; i++)
        planets[i] = NULL;

    planetfile = fopen (filename, "r");
    if (planetfile == NULL)
    {
        fprintf (stderr, "init_planets: No %s file!", filename);
        return (0);
    }
    while (!done)
    {
        buffer[0] = '\0';
        fgets (buffer, BUFF_SIZE, planetfile);
        if (strlen (buffer) == 0)
            done=1;
		  else
		  {
        p_num = popint (buffer, ":");
        p_sec = popint (buffer, ":");
        popstring (buffer, p_name, ":", MAX_NAME_LENGTH);
		  p_type = popint(buffer, ":");
		  p_owner = popint(buffer, ":"); //Owner should be a number!

        planets[p_num - 1] =
            (struct planet *) malloc (sizeof (struct planet));
        planets[p_num - 1]->num = p_num;
        planets[p_num - 1]->name =
            (char *) malloc (strlen (p_name) * sizeof (char));
		  planets[p_num - 1]->creator =
				(char *)malloc(sizeof(char)*MAX_NAME_LENGTH);
		  planets[p_num - 1]->citdl = 
				(struct citadel *)malloc(sizeof(struct citadel));
		  planets[p_num - 1]->pClass = planetTypes[p_type];
        planets[p_num - 1]->sector = p_sec;
        planets[p_num - 1]->owner = p_owner;
        strcpy (planets[p_num - 1]->name, p_name);
        planets[p_num - 1]->type = p_type;

		  popstring(buffer, planets[p_num -1]->creator, ":", MAX_NAME_LENGTH);
		  planets[p_num - 1]->fuelColonist = popint(buffer, ":");
		  planets[p_num - 1]->organicsColonist = popint(buffer, ":");
		  planets[p_num - 1]->equipmentColonist = popint(buffer, ":");
		  planets[p_num - 1]->fuel = popint(buffer, ":");
		  planets[p_num - 1]->organics = popint(buffer, ":");
		  planets[p_num - 1]->equipment = popint(buffer, ":");
		  planets[p_num - 1]->fighters = popint(buffer, ":");
		  planets[p_num - 1]->citdl->level = popint(buffer, ":");
		  popstring(buffer, temp, ":", BUFF_SIZE);
		  planets[p_num - 1]->citdl->treasury = strtoul(temp,NULL,10);
		  planets[p_num - 1]->citdl->militaryReactionLevel = popint(buffer, ":");
		  planets[p_num - 1]->citdl->qCannonAtmosphere = popint(buffer, ":");
 		  planets[p_num - 1]->citdl->qCannonSector = popint(buffer, ":");
 		  planets[p_num - 1]->citdl->planetaryShields = popint(buffer, ":");
		  planets[p_num - 1]->citdl->transporterlvl = popint(buffer, ":");
		  planets[p_num - 1]->citdl->interdictor = popint(buffer, ":");
        
		  insert_planet (planets[p_num - 1], secarray[p_sec - 1], 0);
		  }
    }
    free (p_name);
    return (0);
}


/*
 *	insert_planet(p, s)
 *	returns the sector number it was inserted in, and
 *	-1 if called with a NULL sector
*/
int insert_planet (struct planet *p, struct sector *s, int playernumber)
{
    struct list *p_list, *newp_list;
    if (s == NULL)
    {
        fprintf (stderr, "insert_planet on NULL sector, yo\n");
        return -1;
    }

    newp_list = (struct list *) malloc (sizeof (struct list *));
    newp_list->item = p;
    newp_list->type = planet;
    newp_list->listptr = NULL;

    p_list = s->planets;
    if (p_list != NULL)
    {
        while (p_list->listptr != NULL)
        {
            p_list = p_list->listptr;
        }
        p_list->listptr = newp_list;
    }
    else
        s->planets = newp_list;
    fprintf (stdout, "\t-planet number %d (%s) inserted in sector %d\n", p->num,
             p->name, s->number);
    return s->number;

}

/*
** Create all of the planet types which will be used later.
*/

planetClass planetClassification[NUMBER_OF_PLANET_TYPES];

char *tname[NUMBER_OF_PLANET_TYPES] = {
                                          "Terra",
                                          "(M) - Earth type",
                                          "(L) - Mountainous",
                                          "(O) - Oceanic",
                                          "(K) - Desert Wasteland",
                                          "(H) - Volcanic",
                                          "(U) - Gaseous",
                                          "(C) - Glacial/Ice"
                                      };

char *tdesc[NUMBER_OF_PLANET_TYPES] = {
                                          "Terra",
                                          "(M) - Earth type\n    Thick Oxygen /\n    Nitrogen atmosphere.Specific gravity within 0.7 to 1.3 Earth normal.\n    Random,\n  but mostly manageable weather patterns,\n  with temperatures ranging from 0 to 40 degrees Celsius.Fertile soil,\n  excellent for Organic production.Mineral deposits,\n  very good for Equipment production.Chemical elements good for Fuel Ore.\n    Class M planets are excellent for human colonization and promote an\n    excellent population growth curve as well as a very good population\n    harmony quotient.They have an above average 'habitability band'.\n    Drawbacks include overpopulation problems,\n  political unrest and human - induced destruction of the BioSphere.",
                                          " (L)\n    -Mountainous Thin Oxygen /\n      Nitrogen atmosphere.Specific gravity within 1.0 to 2.2 Earth normal.\n      Weather patterns are wet with temperatures ranging anywhere from -\n      30 to 50 Celsius depending on the width of the habitability band.\n      Excellent Mineral and Ore deposits but harsh conditions only permit avg\n      to below avg Equipment production.Soil is excellent,\n    providing higher than normal organics production.\n      Colonist specialization is necessary to maintain population.Good -\n      very good population harmony quotient.\n      Above average 'habitability band' but only medium population growth.\n      Drawbacks include hazards to equipment and occasional severe weather\n      conditions.",
                                          " (O) -\n      Oceanic Dense Oxygen /\n      Nitrogen atmosphere.Specific gravity within 1.1 to 1.8 Earth normal.\n      Random and occasional violent weather current patterns,\n    with temps ranging from 20 to 50 degrees Celsius.No land mass to speak of,\n    making mining for Ore more difficult.Organics production quite good,\n    (one of the best)\n      but a poor environment for building Equipment.Class O planets are more\n	challenging to habitate,\n      but are almost as safe as class M.Good population growth curve and\n	decent population harmony.Their entire surface is habitable with\n	proper gear with the only drawbacks being the costs to settle and\n	build citadels.",
                                          " (K)\n	-Desert Wasteland Thin Oxygen /\n	  Nitrogen atmosphere.Specific gravity within 0.5 to 1.5 Earth normal.\n	  Weather patterns are mostly dry and hot with temperatures ranging\n	  fron 40 to 140 degrees Celsius.Little area of fertile soil,\n	very bad for Organics.\n	  Very little precious metal making it bad for Equipment production.\n	  Common Chemical traces making it great for Fuel Ore.\n	  Class K worlds are average for humanoid colonization but an arrid\n	  and hot climate requires specialized colonists.\n	  \n	  Narrow habitability band but a generally stable political\n	  environment as the population must depend on each other to survive.\n	  Higher fatality rate than Class M worlds.",
                                          " (H)\n	  -Volcanic Extremely thin Oxygen /\n	    Nitrogen atmospheres.\n	    Specific gravities are within 0.8 to 2.6 Earth normal.\n	    Climate patterns are violent with temperatures from 45 to 400\n	    degrees Celcius.Full life support required for colonization.\n	    Zero workable soil and harsh conditions make Organics production\n	    impossible.\n	    \n	    Good trace elements for equip but conditions make production a\n	    gamble at best.\n	    Excellent Ore production possibilities as material is often\n	    ejected by volcanic activity and found on the surface.\n	    Very dangerous for colony growth as unstable planetary crusts often\n	    lead to the complete loss of a colony.\n	    \n	    The Federation has been known to use Class H planets for defense\n	    of key sectors due to their large Ore base.",
                                          " (U)\n	    -Gaseous Heavy ranging to very thin atmospheres consisting of\n	      various elements,\n	    mostly comprised of helium or of hydrogen.\n	      Specific gravities can range from 0.2 to 8.0 of Earth normal.\n	      Climate patterns usually extremely violent with temps ranging\n	      from -\n	      200 to 400 Celsius.Full life support required at all times.\n	      No production can sustain itself on a Class U planet.\n	      Some miners have hinted at very valuable products that they have\n	      extracted from Class U worlds but the Federation does not have\n       them in its 'Official Guide to Mining'.\n	      \n	      Class U planets are not recommended for colonization as the\n	      environment is harsher than being in space itself.",
                                          " (C)\n	      -Glacial / Ice Extremely thin Oxygen -\n		Nitrogen atmospheres.Specific gravity from 0.5 to 1.7 Earth\n		normal.Meteorologically unstable causing violent conditions.\n		Temps range from - 10 to -\n		190 degrees Celsius.\n		Full life support necessary for colonies and death rates are high.No\n		workable soil base so hydroponic Organics are limited.\n		Modest mineral and chemicals exist so production of Ore and Equipment\n		will be below average to none.\n		Class C planets NOT recommended for colonization,\n	      their violent condition makes it extremely hazardous.\n		\n		Some Class C planets have been adopted by the Federation and\n		used as prison colonies with very effective results."
                                      };

int ciduptm[NUMBER_OF_PLANET_TYPES][MAX_CITADEL_LEVEL] = {
            {-1, -1, -1, -1, -1, -1, -1},	/* terra */
            {4, 4, 5, 10, 5, 15},		/* M */
            {2, 5, 5, 8, 5, 12},		/* L */
            {6, 5, 8, 5, 4, 8},		/* O */
            {6, 5, 8, 5, 4, 8},		/* K */
            {4, 5, 8, 12, 18, 8},		/* H */
            {8, 4, 5, 5, 4, 8},		/* U */
            {5, 5, 7, 5, 4, 8},		/* C */
        };


int cidupcolonist[NUMBER_OF_PLANET_TYPES][MAX_CITADEL_LEVEL] = {
            {1000000, 1000000, 1000000, 1000000, 1000000, 1000000},	/* terra */
            {1000000, 2000000, 4000000, 6000000, 6000000, 6000000},	/* M */
            {400000, 1400000, 3600000, 5600000, 7000000, 5600000},	/* L */
            {1400000, 2400000, 4400000, 7000000, 8000000, 7000000},	/* O */
            {1000000, 2400000, 4000000, 7000000, 8000000, 7000000},	/* K */
            {800000, 1600000, 4400000, 7000000, 10000000, 7000000},	/* H */
            {3000000, 3000000, 8000000, 6000000, 8000000, 6000000},	/* U */
            {1000000, 24000000, 4400000, 6600000, 9000000, 6600000}	/* C */
        };


int cidupore[NUMBER_OF_PLANET_TYPES][MAX_CITADEL_LEVEL] = {
            {-1, -1, -1, -1, -1, -1, -1},	/* terra */
            {300, 200, 500, 1000, 300, 1000},	/* M */
            {150, 200, 600, 1000, 300, 1000},	/* L */
            {500, 200, 600, 700, 300, 700},	/* O */
            {400, 300, 700, 700, 300, 700},	/* K */
            {500, 300, 1200, 2000, 3000, 2000},	/* H */
            {1200, 300, 500, 500, 200, 500},	/* U */
            {400, 300, 600, 700, 300, 700},	/* C */
        };

int ciduporganic[NUMBER_OF_PLANET_TYPES][MAX_CITADEL_LEVEL] = {
            {-1, -1, -1, -1, -1, -1, -1},	/* terra */
            {200, 50, 250, 1200, 400, 1200},	/* M */
            {100, 50, 250, 1200, 400, 1200},	/* L */
            {200, 50, 400, 900, 400, 900},	/* O */
            {300, 80, 900, 900, 400, 900},	/* K */
            {300, 100, 400, 2000, 1200, 2000},	/* H */
            {400, 100, 500, 200, 200, 200},	/* U */
            {300, 80, 400, 900, 400, 900},	/* C */
        };

int cidupeq[NUMBER_OF_PLANET_TYPES][MAX_CITADEL_LEVEL] = {
            {-1, -1, -1, -1, -1, -1, -1},	/* terra */
            {250, 250, 500, 1000, 1000, 2000},	/* M */
            {150, 250, 700, 1000, 1000, 2000},	/* L */
            {400, 300, 650, 800, 1000, 1600},	/* O */
            {600, 400, 800, 800, 1000, 1600},	/* K */
            {600, 400, 1500, 2500, 2000, 5000},	/* H */
            {2500, 400, 2000, 600, 600, 1200},	/* U */
            {600, 400, 650, 700, 1000, 1400},	/* C */
        };


int numColonistPerProduct[NUMBER_OF_PLANET_TYPES][MAX_CITADEL_LEVEL] = {
            {-1, -1, -1},			/* terra */
            {100000, 100000, 100000},	/* M */
            {200000, 200000, 100000},	/* L */
            {100000, 1000000, 50000},	/* O */
            {200000, 50000, 10000},	/* K */
            {1000000, 10000, 100000},	/* H */
            {10000, 10000, 10000},	/* U */
            {20000, 50000, 10000}		/* C */
        };

int PTfighters[NUMBER_OF_PLANET_TYPES] = {
            1000000, 1000000, 1000000, 1000000, 1000000, 1000000, 1000000
        };
int PTfuelProduction[NUMBER_OF_PLANET_TYPES] = {
            -1, 3, 2, 20, 2, 1, -1, 50
        };
int PTorganicsProduction[NUMBER_OF_PLANET_TYPES] = {
            -1, 7, 5, 2, 100, -1, -1, 100
        };
int PTequipmentProduction[NUMBER_OF_PLANET_TYPES] = {
            -1, 13.50, 100, 500, 50, -1, 500
        };
int PTfighterProduction[NUMBER_OF_PLANET_TYPES] = {
            -1, 30, 24, 30, -1, 30, -1, -1
        };
float PTbreeding[NUMBER_OF_PLANET_TYPES] = {
            0.75, 0.15, 0.15, 0.11, 0.05, 0.03, -0.01, -0.01
        };



/*
void init_planetClassification(void)
{
  int x,a;
 
  for (x=1;x<NUMBER_OF_PLANET_TYPES;x++)
    {
      planetClassification[x]->typeName = &tname[x];
      planetClassification[x]->typeDescription = &tdesc[x];
      for(a=0;a<MAX_CIDADEL_LEVEL;a++)
	{
	  planetClassification[x]->citadelUpgradeTime[a];
	  planetClassification[x]->citadelUpgradeOre[a];
	  planetClassification[x]->citadelUpgradeOrganics[a];
	  planetClassification[x]->citadelUpgradeEquipment[a];
	  planetClassification[x]->citadelUpgradeColonist[a];
	}
      for(a=0;a<3;a++)
	planetClassification[x]->maxColonist[a];
 
      planetClassification[x]->fighters = ;
      planetClassification[x]->fuelProduction = ;   
      planetClassification[x]->organicsProduction = ;  
      planetClassification[x]->equipmentProduction = ;  
      planetClassification[x]->fighterProduction = ;   
      planetClassification[x]->breeding = ;
 
    }
 
}
 
*/



/*
struct planetType_struct 
{
  char *typeDescription;
  char *typeName;
  int citadelUpgradeTime[MAX_CIDADEL_LEVEL];
  int citadelUpgradeOre[MAX_CIDADEL_LEVEL];
  int citadelUpgradeOrganics[MAX_CIDADEL_LEVEL];
  int citadelUpgradeEquipment[MAX_CIDADEL_LEVEL];
  int citadelUpgradeColonist[MAX_CIDADEL_LEVEL];
  int maxColonist[3]; 
  int fighters;
  float fuelProduction;   
  float organicsProduction;  
  float equipmentProduction;  
  float fighterProduction;   
  float breeding;
};
*/
void save_planetinfo(char *filename)
{
	FILE *planetinfo;
	char *stufftosave = (char *)malloc(sizeof(char)*BUFF_SIZE);
	int done=0;
	int index=0;
	int loop=0;

	planetinfo = fopen(filename, "w");

	for (index=0; index < configdata->number_of_planet_types; index++)
	{
	strcpy(stufftosave, "\0");
	addstring(stufftosave, planetTypes[index]->typeClass,':', BUFF_SIZE);
	addstring(stufftosave, planetTypes[index]->typeName, ':', BUFF_SIZE);
	for (loop=0;loop<MAX_CITADEL_LEVEL-1; loop++)
	{
		addint(stufftosave, planetTypes[index]->citadelUpgradeTime[loop],
							 ',', BUFF_SIZE);
	}
	addint(stufftosave, planetTypes[index]->citadelUpgradeTime[MAX_CITADEL_LEVEL-1], ':', BUFF_SIZE);
	for (loop=0;loop<MAX_CITADEL_LEVEL-1; loop++)
	{
		addint(stufftosave, planetTypes[index]->citadelUpgradeOre[loop],
							 ',', BUFF_SIZE);
	}
	addint(stufftosave, planetTypes[index]->citadelUpgradeOre[MAX_CITADEL_LEVEL-1], ':', BUFF_SIZE);
	for (loop=0;loop<MAX_CITADEL_LEVEL-1; loop++)
	{
		addint(stufftosave, planetTypes[index]->citadelUpgradeOrganics[loop],
							 ',', BUFF_SIZE);
	}
	addint(stufftosave, planetTypes[index]->citadelUpgradeOrganics[MAX_CITADEL_LEVEL-1], ':', BUFF_SIZE);
	for (loop=0;loop<MAX_CITADEL_LEVEL-1; loop++)
	{
		addint(stufftosave, planetTypes[index]->citadelUpgradeEquipment[loop],
							 ',', BUFF_SIZE);
	}
		addint(stufftosave, planetTypes[index]->citadelUpgradeEquipment[MAX_CITADEL_LEVEL-1], ':', BUFF_SIZE);
	for (loop=0;loop<MAX_CITADEL_LEVEL-1; loop++)
	{
		addint(stufftosave, planetTypes[index]->citadelUpgradeColonist[loop],
							 ',', BUFF_SIZE);
	}
	addint(stufftosave, planetTypes[index]->citadelUpgradeColonist[MAX_CITADEL_LEVEL-1], ':', BUFF_SIZE);
	for (loop=0;loop<2; loop++)
	{
		addint(stufftosave, planetTypes[index]->maxColonist[loop],
							 ',', BUFF_SIZE);
	}
	addint(stufftosave, planetTypes[index]->citadelUpgradeTime[2], ':', BUFF_SIZE);
	addint(stufftosave, planetTypes[index]->fuelProduction, ':', BUFF_SIZE);
	addint(stufftosave, planetTypes[index]->organicsProduction, ':', BUFF_SIZE);
	addint(stufftosave, planetTypes[index]->equipmentProduction, ':', BUFF_SIZE);
	addint(stufftosave, planetTypes[index]->fighterProduction, ':', BUFF_SIZE);
	addint(stufftosave, planetTypes[index]->maxore, ':', BUFF_SIZE);
	addint(stufftosave, planetTypes[index]->maxorganics, ':', BUFF_SIZE);
	addint(stufftosave, planetTypes[index]->maxequipment, ':', BUFF_SIZE);
	addint(stufftosave, planetTypes[index]->maxfighters, ':', BUFF_SIZE);
	addint(stufftosave, (int)(planetTypes[index]->breeding*1000), ':', BUFF_SIZE);

	for (loop=1; loop < 399 - strlen(stufftosave); loop++)
		strcat(stufftosave, " ");
	strcat(stufftosave, "\n");
	fprintf(planetinfo, "%s", stufftosave);
	}
	fclose(planetinfo);
	free(stufftosave);
}
void init_planetinfo(char *filename)
{
	FILE *planetinfo;
	char *buffer = (char *)malloc(sizeof(char)*BUFF_SIZE);
	char *temp = (char *)malloc(sizeof(char)*BUFF_SIZE);
	int done=0;
	int index=0;
	int loop=0;

	planetTypes = (planetClass **)
		malloc(sizeof(planetClass *)*configdata->number_of_planet_types);
	
	planetinfo = fopen(filename, "r");
	if (planetinfo == NULL)
	{
		fprintf(stderr, "init_planetinfo: No %s file!", filename);
		exit(-1);
	}
	while(!done)
	{
		buffer[0] = '\0';
		fgets(buffer,BUFF_SIZE, planetinfo);
		if (strlen(buffer)==0)
		{
			done = 1;
		}
		else
		{
			planetTypes[index] =
				(planetClass *)malloc(sizeof(planetClass));
			planetTypes[index]->typeClass = 
				(char *)malloc(sizeof(char)*MAX_NAME_LENGTH*2);
			planetTypes[index]->typeName =
				(char *)malloc(sizeof(char)*MAX_NAME_LENGTH*2);

			popstring(buffer,planetTypes[index]->typeClass, ":", MAX_NAME_LENGTH*2);
			popstring(buffer,planetTypes[index]->typeName, ":", MAX_NAME_LENGTH*2);
			temp[0] = '\0';
			popstring(buffer,temp,":", BUFF_SIZE);
			for (loop=0;loop<MAX_CITADEL_LEVEL;loop++)
			{
				planetTypes[index]->citadelUpgradeTime[loop] = 
						  popint(temp, ",");
			}
			temp[0] = '\0';
			popstring(buffer,temp,":", BUFF_SIZE);
			for (loop=0;loop<MAX_CITADEL_LEVEL;loop++)
			{
				planetTypes[index]->citadelUpgradeOre[loop] = 
						  popint(temp, ",");
			}
			temp[0] = '\0';
			popstring(buffer,temp,":", BUFF_SIZE);
			for (loop=0;loop<MAX_CITADEL_LEVEL;loop++)
			{
				planetTypes[index]->citadelUpgradeOrganics[loop] = 
						  popint(temp, ",");
			}
			temp[0] = '\0';
			popstring(buffer,temp,":", BUFF_SIZE);
			for (loop=0;loop<MAX_CITADEL_LEVEL;loop++)
			{
				planetTypes[index]->citadelUpgradeEquipment[loop] = 
						  popint(temp, ",");
			}
			temp[0] = '\0';
			popstring(buffer,temp,":", BUFF_SIZE);
			for (loop=0;loop<MAX_CITADEL_LEVEL;loop++)
			{
				planetTypes[index]->citadelUpgradeColonist[loop] = 
						  popint(temp, ",");
			}
			temp[0] = '\0';
			popstring(buffer,temp,":", BUFF_SIZE);
			for (loop=0;loop< 3;loop++)
			{
				planetTypes[index]->maxColonist[loop] = 
						  popint(temp, ",");
			}
			
			planetTypes[index]->fuelProduction = popint(buffer, ":");
			planetTypes[index]->organicsProduction = popint(buffer, ":");
			planetTypes[index]->equipmentProduction = popint(buffer, ":");
			planetTypes[index]->fighterProduction = popint(buffer, ":");
			planetTypes[index]->maxore = popint(buffer, ":");
			planetTypes[index]->maxorganics = popint(buffer, ":");
			planetTypes[index]->maxequipment = popint(buffer, ":");
			planetTypes[index]->maxfighters = popint(buffer, ":");
			
			planetTypes[index]->breeding = (float)popint(buffer, ":")/1000.0;

		}
		index++;
	}
	fclose(planetinfo);
}
