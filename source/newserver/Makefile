CC=gcc
#FLAGS=-Wall
LIBS=-lpthread -ldb

all: 	server

server: server.o listen.o backend.o console.o player.o
		${CC} ${FLAGS} -o server server.c listen.o backend.o console.o player.o ${LIBS}

server.o: server.c
		${CC} ${FLAGS} -c server.c

listen.o: listen.c
		${CC} ${FLAGS} -c listen.c

backend.o: backend.c
		${CC} ${FLAGS} -c backend.c 

console.o:	console.c
		${CC} ${FLAGS} -c console.c

player.o:	player.c
		${CC} ${FLAGS} -c player.c

universe.o:	universe.c
		${CC} ${FLAGS} -c universe.c

clean:
		rm -rf server *.o
