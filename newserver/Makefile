CC=gcc
#FLAGS=-Wall
LIBS=-lpthread -lm

all: 	server

server: server.o listen.o backend.o console.o player.o common.o universe.o msgqueue.o player_interaction.o sysop_interaction.o hashtable.o parse.o serveractions.o shipinfo.o planet.o
		${CC} ${FLAGS} -o server server.c listen.o backend.o console.o player.o common.o universe.o msgqueue.o player_interaction.o sysop_interaction.o hashtable.o parse.o serveractions.o shipinfo.o planet.o ${LIBS}

server.o: server.c
		${CC} ${FLAGS} -c server.c

listen.o: listen.c
		${CC} ${FLAGS} -c listen.c

backend.o: backend.c
		${CC} ${FLAGS} -c backend.c

console.o:	console.c
		${CC} ${FLAGS} -c console.c

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

hashtable.o : universe.h hashtable.h hashtable.c
	gcc $(FLAGS) -c -o hashtable.o hashtable.c

parse.o : parse.h parse.c
	gcc $(FLAGS) -c -o parse.o parse.c

shipinfo.o : shipinfo.h shipinfo.c
	gcc $(FLAGS) -c -o shipinfo.o shipinfo.c

serveractions.o : shipinfo.h common.h parse.h msgqueue.h universe.h hashtable.h portinfo.h serveractions.h serveractions.c planet.h planet.c
	gcc $(FLAGS) -c -o serveractions.o serveractions.c


clean:
		rm -rf server *.o
