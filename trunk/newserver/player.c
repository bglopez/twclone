#include "player.h"
#include "unistd.h"

void reject_player(struct connection *cx)
{
	write(cx->connfd, g_reject_message, strlen(g_reject_message));
	close(cx->connfd);
}

void join_game(struct connection *cx)
{
   if(cx) write(cx->connfd, "welcome to the server.\0", 22);
//	retreive player info from data file
//	put info into cx
//	print sector
//	go interactive
}
