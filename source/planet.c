#include <stdio.h>
#include <stdlib.h>
#include "planet.h"

/*
 *	init_planets(filename, secarray)
 *	loads planet info from file.  returns number
 *	of planets in the universe when done
 */

int
init_planets (char *filename, struct sector *secarray[])
{
  FILE *planetfile;
  int i, p_num, p_sec, p_type;
  char *p_name, *p_owner, p_ownertype, dummy;
  int count;
  p_name = (char *) malloc (sizeof (char) * (MAX_NAME_LENGTH + 1));
  p_owner = (char *) malloc (sizeof (char) * (MAX_NAME_LENGTH + 1));

  for (i = 0; i < MAX_TOTAL_PLANETS; i++)
    planets[i] = NULL;

  planetfile = fopen (filename, "r");
  while ((count =
	  fscanf (planetfile, "%d:%d:%[^:]:%c:%[^:]:%d:%c", &p_num, &p_sec,
		  p_name, &p_ownertype, p_owner, &p_type, &dummy))
	 && count > 6)
    {

      planets[p_num - 1] =
	(struct planet *) malloc (sizeof (struct planet *));
      planets[p_num - 1]->num = p_num;
      planets[p_num - 1]->name =
	(char *) malloc (strlen (p_name) * sizeof (char));
      planets[p_num - 1]->owner =
	(char *) malloc (strlen (p_owner) * sizeof (char));
      planets[p_num - 1]->name = p_name;
      planets[p_num - 1]->owner = p_owner;
      planets[p_num - 1]->ownertype = p_ownertype;
      planets[p_num - 1]->type = p_type;
      insert_planet (planets[p_num - 1], secarray[p_sec - 1], 0);
    }
	return(0);
}


/*
 *	insert_planet(p, s)
 *	returns the sector number it was inserted in, and
 *	-1 if called with a NULL sector
*/
int
insert_planet(struct planet *p, struct sector *s, int playernumber)
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
  "(M) - Earth type
    Thick Oxygen /
    Nitrogen atmosphere.Specific gravity within 0.7 to 1.3 Earth normal.
    Random, but mostly manageable weather patterns,
    with temperatures ranging from 0 to 40 degrees Celsius.Fertile soil,
    excellent for Organic production.Mineral deposits,
    very good for Equipment production.Chemical elements good for Fuel Ore.
    Class M planets are excellent for human colonization and promote an
    excellent population growth curve as well as a very good population
    harmony quotient.They have an above average 'habitability band'.
    Drawbacks include overpopulation problems,
    political unrest and human -
    induced destruction of the BioSphere.", "(L) - Mountainous Thin Oxygen /
    Nitrogen atmosphere.Specific gravity within 1.0 to 2.2 Earth normal.
    Weather patterns are wet with temperatures ranging anywhere from -
    30 to 50 Celsius depending on the width of the habitability band.
    Excellent Mineral and Ore deposits but harsh conditions only permit avg to
    below avg Equipment production.Soil is excellent,
    providing higher than normal organics production.
    Colonist specialization is necessary to maintain population.Good -
    very good population harmony quotient.
    Above average 'habitability band' but only medium population growth.
    Drawbacks include hazards to equipment and occasional severe weather
    conditions.", "(O) - Oceanic Dense Oxygen /
    Nitrogen atmosphere.Specific gravity within 1.1 to 1.8 Earth normal.
    Random and occasional violent weather current patterns,
    with temps ranging from 20 to 50 degrees Celsius.No land mass to speak of,
    making mining for Ore more difficult.Organics production quite good,
    (one of the best)
    but a poor environment for building Equipment.
      Class O planets are more challenging to habitate,
      but are almost as safe as class M.
      Good population growth curve and decent population harmony.
      Their entire surface is habitable with proper gear with the only
      drawbacks being the costs to settle and build citadels.
      ", "(K) - Desert Wasteland Thin Oxygen /
      Nitrogen atmosphere.Specific gravity within 0.5 to 1.5 Earth normal.
      Weather patterns are mostly dry and hot with temperatures ranging fron
      40 to 140 degrees Celsius.Little area of fertile soil,
      very bad for Organics.
      Very little precious metal making it bad for Equipment production.
      Common Chemical traces making it great for Fuel Ore.
      Class K worlds are average for humanoid colonization but an arrid and
      hot climate requires specialized colonists.
      Narrow habitability band but a generally stable political environment as
      the population must depend on each other to survive.
      Higher fatality rate than Class M worlds.
      ", "(H) - Volcanic Extremely thin Oxygen /
      Nitrogen atmospheres.
      Specific gravities are within 0.8 to 2.6 Earth normal.
      Climate patterns are violent with temperatures from 45 to 400 degrees
      Celcius.Full life support required for colonization.
      Zero workable soil and harsh conditions make Organics production
      impossible.
      Good trace elements for equip but conditions make production a gamble at
      best.
      Excellent Ore production possibilities as material is often ejected by
      volcanic activity and found on the surface.
      Very dangerous for colony growth as unstable planetary crusts often lead
      to the complete loss of a colony.
      The Federation has been known to use Class H planets for defense of key
      sectors due to their large Ore base.
      ", "(U) - Gaseous Heavy ranging to very thin atmospheres consisting of
      various elements,
      mostly comprised of helium or of hydrogen.
      Specific gravities can range from 0.2 to 8.0 of Earth normal.
      Climate patterns usually extremely violent with temps ranging from -
      200 to 400 Celsius.Full life support required at all times.
      No production can sustain itself on a Class U planet.
      Some miners have hinted at very valuable products that they have
      extracted from Class U worlds but the Federation does not have them in
      its 'Official Guide to Mining'.
      Class U planets are not recommended for colonization as the environment
      is harsher than being in space itself.
      ", "(C) - Glacial/Ice Extremely thin Oxygen -
      Nitrogen atmospheres.Specific gravity from 0.5 to 1.7 Earth normal.
      Meteorologically unstable causing violent conditions.Temps range from -
      10 to -
      190 degrees Celsius.
      Full life support necessary for colonies and death rates are high.
      No workable soil base so hydroponic Organics are limited.
      Modest mineral and chemicals exist so production of Ore and Equipment
      will be below average to none.
      Class C planets NOT recommended for colonization,
      their violent condition makes it extremely hazardous.
      Some Class C planets have been adopted by the Federation and used as
      prison colonies with very effective results."
    };

    int ciduptm[NUMBER_OF_PLANET_TYPES]
	[MAX_CIDADEL_LEVEL] = {
	{-1, -1, -1, -1, -1, -1, -1},	/* terra */
	{4, 4, 5, 10, 5, 15},	/* M */
	{2, 5, 5, 8, 5, 12},	/* L */
	{6, 5, 8, 5, 4, 8},	/* O */
	{6, 5, 8, 5, 4, 8},	/* K */
	{4, 5, 8, 12, 18, 8},	/* H */
	{8, 4, 5, 5, 4, 8},	/* U */
	{5, 5, 7, 5, 4, 8},	/* C */
	};


  int cidupcolonist[NUMBER_OF_PLANET_TYPES][MAX_CIDADEL_LEVEL] = {
    {1000000, 1000000, 1000000, 1000000, 1000000, 1000000},	/* terra */
    {1000000, 2000000, 4000000, 6000000, 6000000, 6000000},	/* M */
    {400000, 1400000, 3600000, 5600000, 7000000, 5600000},	/* L */
    {1400000, 2400000, 4400000, 7000000, 8000000, 7000000},	/* O */
    {1000000, 2400000, 4000000, 7000000, 8000000, 7000000},	/* K */
    {800000, 1600000, 4400000, 7000000, 10000000, 7000000},	/* H */
    {3000000, 3000000, 8000000, 6000000, 8000000, 6000000},	/* U */
    {1000000, 24000000, 4400000, 6600000, 9000000, 6600000}	/* C */
  };


  int cidupore[NUMBER_OF_PLANET_TYPES][MAX_CIDADEL_LEVEL] = {
    {-1, -1, -1, -1, -1, -1, -1},	/* terra */
    {300, 200, 500, 1000, 300, 1000},	/* M */
    {150, 200, 600, 1000, 300, 1000},	/* L */
    {500, 200, 600, 700, 300, 700},	/* O */
    {400, 300, 700, 700, 300, 700},	/* K */
    {500, 300, 1200, 2000, 3000, 2000},	/* H */
    {1200, 300, 500, 500, 200, 500},	/* U */
    {400, 300, 600, 700, 300, 700},	/* C */
  };

  int ciduporganic[NUMBER_OF_PLANET_TYPES][MAX_CIDADEL_LEVEL] = {
    {-1, -1, -1, -1, -1, -1, -1},	/* terra */
    {200, 50, 250, 1200, 400, 1200},	/* M */
    {100, 50, 250, 1200, 400, 1200},	/* L */
    {200, 50, 400, 900, 400, 900},	/* O */
    {300, 80, 900, 900, 400, 900},	/* K */
    {300, 100, 400, 2000, 1200, 2000},	/* H */
    {400, 100, 500, 200, 200, 200},	/* U */
    {300, 80, 400, 900, 400, 900},	/* C */
  };

  int cidupeq[NUMBER_OF_PLANET_TYPES][MAX_CIDADEL_LEVEL] = {
    {-1, -1, -1, -1, -1, -1, -1},	/* terra */
    {250, 250, 500, 1000, 1000, 2000},	/* M */
    {150, 250, 700, 1000, 1000, 2000},	/* L */
    {400, 300, 650, 800, 1000, 1600},	/* O */
    {600, 400, 800, 800, 1000, 1600},	/* K */
    {600, 400, 1500, 2500, 2000, 5000},	/* H */
    {2500, 400, 2000, 600, 600, 1200},	/* U */
    {600, 400, 650, 700, 1000, 1400},	/* C */
  };


  int numColonistPerProduct[NUMBER_OF_PLANET_TYPES][MAX_CIDADEL_LEVEL] = {
    {-1, -1, -1},		/* terra */
    {100000, 100000, 100000},	/* M */
    {200000, 200000, 100000},	/* L */
    {100000, 1000000, 50000},	/* O */
    {200000, 50000, 10000},	/* K */
    {1000000, 10000, 100000},	/* H */
    {10000, 10000, 10000},	/* U */
    {20000, 50000, 10000}	/* C */
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
