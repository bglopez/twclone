#include <stdio.h>
#include "planet.h"

/*
 *	init_planets(filename, secarray)
 *	loads planet info from file.  returns number
 *	of planets in the universe when done
 */

int init_planets(char *filename, struct sector *secarray[]){
	FILE *planetfile;
	struct sector *foo;
	int i, p_num, p_sec, p_type;
	char *p_name, *p_owner, p_ownertype, dummy;
	int count;
	p_name=(char*)malloc(sizeof(char)*(MAX_NAME_LENGTH+1));
	p_owner=(char*)malloc(sizeof(char)*(MAX_NAME_LENGTH+1));

	for(i=0;i<MAX_TOTAL_PLANETS;i++) planets[i]=NULL;

	planetfile = fopen(filename, "r");
	while((count=fscanf(planetfile, "%d:%d:%[^:]:%c:%[^:]:%d:%c", &p_num, &p_sec, p_name, &p_ownertype, p_owner, &p_type, &dummy)) && count > 6){

	planets[p_num-1] = (struct planet*)malloc(sizeof(struct planet*));
	planets[p_num-1]->num = p_num;
	planets[p_num-1]->name = (char*)malloc(strlen(p_name)*sizeof(char));
	planets[p_num-1]->owner = (char*)malloc(strlen(p_owner)*sizeof(char));
	planets[p_num-1]->name = p_name;
	planets[p_num-1]->owner = p_owner;
	planets[p_num-1]->ownertype = p_ownertype;
	planets[p_num-1]->type = p_type; 
	insert_planet(planets[p_num-1], secarray[p_sec-1]);	
	}

}


/*
 *	insert_planet(p, s)
 *	returns the sector number it was inserted in, and
 *	-1 if called with a NULL sector
*/
int insert_planet(struct planet *p, struct sector *s){
	struct list *p_list;
	if(s==NULL){
		fprintf(stderr, "insert_planet on NULL sector, yo\n");
		return -1;
	}
	p_list=s->planets;
	while(p_list != NULL) p_list = p_list->listptr;
	p_list=(struct list*)malloc(sizeof(struct list*));
	p_list->item = p;
	p_list->type = planet;
	p_list->listptr = NULL;
	fprintf(stdout, "\t-planet number %d (%s) inserted in sector %d\n", p->num, p->name, s->number);
	return s->number;
}
