FLAGS = -g -Wall

all : server test_server test_client client bigbang

common.o : common.h common.c
	gcc $(FLAGS) -c -o common.o common.c

universe.o : parse.h common.h universe.h universe.c
	gcc $(FLAGS) -c -o universe.o universe.c

msgqueue.o : common.h msgqueue.h msgqueue.c
	gcc $(FLAGS) -c -o msgqueue.o msgqueue.c

player_interaction.o : parse.h common.h msgqueue.h player_interaction.h player_interaction.c
	gcc $(FLAGS) -c -o player_interaction.o player_interaction.c

sysop_interaction.o : common.h msgqueue.h player_interaction.h sysop_interaction.h sysop_interaction.c
	gcc $(FLAGS) -c -o sysop_interaction.o sysop_interaction.c

maint.o : universe.h config.h shipinfo.h hashtable.h msgqueue.h common.h maint.h maint.c serveractions.h
	gcc $(FLAGS) -c -o maint.o maint.c

hashtable.o : universe.h hashtable.h hashtable.c
	gcc $(FLAGS) -c -o hashtable.o hashtable.c

parse.o : parse.h parse.c
	gcc $(FLAGS) -c -o parse.o parse.c

shipinfo.o : shipinfo.h shipinfo.c
	gcc $(FLAGS) -c -o shipinfo.o shipinfo.c

serveractions.o : shipinfo.h common.h parse.h msgqueue.h universe.h hashtable.h portinfo.h serveractions.h serveractions.c planet.h planet.c config.h
	gcc $(FLAGS) -c -o serveractions.o serveractions.c

config.o : common.h parse.h config.h config.c
	gcc $(FLAGS) -c -o config.o config.c
	
server : server.c common.o universe.o msgqueue.o player_interaction.o sysop_interaction.o hashtable.o parse.o serveractions.o shipinfo.o planet.o config.o maint.o
	gcc $(FLAGS) -lm -o server server.c -lpthread common.o universe.o msgqueue.o player_interaction.o sysop_interaction.o hashtable.o parse.o serveractions.o shipinfo.o planet.o config.o maint.o

client : client.c common.o client.h ansi.h common.h parse.h
	gcc $(FLAGS) -lm -o client client.c common.o parse.o 

test_server : test_server.c common.o 
	gcc $(FLAGS) -o test_server test_server.c -lpthread common.o 

test_client : common.o test_client.c
	gcc $(FLAGS) -o test_client test_client.c -lpthread common.o

bigbang : universe.h bigbang.c
	gcc $(FLAGS) -o bigbang bigbang.c parse.o config.o 
clean : 
	rm -f *.o server test_client test_server client bigbang

