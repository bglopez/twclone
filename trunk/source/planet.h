#ifndef PLANET_H_
#define PLANET_H_

#include "universe.h"

#ifndef RAND_MAX
#define RAND_MAX 1
#endif

#define MAX_SAFE_PLANETS 5
struct planet *planets[MAX_TOTAL_PLANETS];

int init_planets(char *filename, struct sector *secarray[]);
int insert_planet(struct planet *p, struct sector *s);

#endif
