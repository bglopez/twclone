#include "listen.h"

#include <stdio.h>
#include <sys/types.h>	// socket, bind
#include <sys/socket.h>	// socket, tcp, bind, listen
#include <sys/poll.h>	// poll
#include <netinet/in.h> // tcp
#include <netdb.h>	// gethostbyname
#include <unistd.h>	// close, write
#include <string.h>	// memcpy, memset

#include "player.h"

#ifdef __linux__
#define SOCKLEN_TYPE socklen_t 
#else
#define SOCKLEN_TYPE int
#endif

static char *g_login_prompt =
"Welcome to the Tradewars 2k GNU server. 

 Login (or type 'NEW'): ";

static char *g_passwd_prompt = " Password: ";

int init_socket(struct sockaddr_in *addr, int port);
struct connection* next_connection(int sock_desc, struct sockaddr_in *addr);
void* listen_thread(void *arg);
void* player_login(void *arg);

int init_socket(struct sockaddr_in *addr, int port)
{
	int sockfd, conn=1, addr_length;
	
	SOCKLEN_TYPE sock_len;

	addr_length=sizeof(struct sockaddr_in);
	inet_aton("0.0.0.0", &(addr->sin_addr));
	addr->sin_family = AF_INET;
	addr->sin_port = htons((short)port);

	sock_len=sizeof(SOCKLEN_TYPE);
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&conn, sock_len);

	bind(sockfd, (struct sockaddr*)addr, addr_length);
	if (listen(sockfd, 10)<0) printf("listen fuxord\n");

	return sockfd;
}


struct connection* next_connection(int sockfd, struct sockaddr_in *addr)
{
	int addr_length=sizeof(struct sockaddr_in);
	struct connection *ncon;
	ncon=(struct connection*)malloc(sizeof(struct connection));

	ncon->connfd = accept(sockfd, (struct sockaddr*)addr, &addr_length);
	printf("recieved connection from %s\n", inet_ntoa(addr->sin_addr));

	return ncon;
}

void init_listen_thread(pthread_t *tid)
{
		fprintf(stderr, "starting listen thread...\n");
		pthread_create(tid, NULL, listen_thread, NULL);
}
		
void* listen_thread(void *arg)
{
	struct connection *cx;
	struct sockaddr_in addr;
	pthread_t tid;
	int sockfd = init_socket(&addr, 9999);
	while(1)
	{
		cx = next_connection(sockfd, &addr);
		pthread_create(&tid, NULL, player_login, cx);
	}
	pthread_exit(0);
}

void* player_login(void *arg)
{
	struct connection *cn = (struct connection*)arg;
	
	write(cn->connfd, g_login_prompt, strlen(g_login_prompt));
	cn->ibuff_len = recv(cn->connfd, (void *)cn->ibuff, MAX_IBUFFSIZE, 0);
	
	fprintf(stderr, "got a login for %s\n", cn->ibuff);
	
	write(cn->connfd, g_passwd_prompt, strlen(g_passwd_prompt));
	cn->ibuff_len = recv(cn->connfd, (void *)cn->ibuff, MAX_IBUFFSIZE, 0);
	join_game(cn);
	
	return NULL;
}
/*
ncon->ufds.fd = ncon->connfd;
ncon->ufds.events = POLLIN;
while(poll(&(ncon->ufds), 1, -1)){
	if((ncon->ufds.revents & POLLIN)==POLLIN) 
		ncon->ibuff_len=recv(ncon->connfd, ncon->ibuff, MAX_IBUFFSIZE, 0);
	printf("%s", ncon->ibuff);
	printf("(only wanted first %d chars)\n", ncon->ibuff_len);
}
*/
