#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "listen.h"

void reject_player(struct connection *cx);
void join_game(struct connection *cx);

static char *g_reject_message = "Sorry, that password was incorrect.
Good-Bye...\n";


#endif
