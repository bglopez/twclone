SHELL = /bin/sh
CC = gcc
NSPRINC = ../nspr-4.2/include
NSPRLIB = ../nspr-4.2/lib
#CFLAGS = -g -O2 -Wall -pedantic -I $(NSPRINC)     
CFLAGS = -g -O2 -Wall -I $(NSPRINC)     
LFLAGS = -L/usr/local/lib -lpthread -ldl
PROGRAM = serverlauncher
SOURCES = server.c \
	  network.c \
	  misc.c \
	  xmlout.c \
	  misc_util.c 


OBJECTS= $(SOURCES.c:.c=.o)


all:$(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) -o $@ $(CFLAGS) $(SOURCES) $(NSPRLIB)/libnspr4.so  $(NSPRLIB)/libplc4.so  $(NSPRLIB)/libplds4.so

clean:
	rm -f $(PROGRAM) $(OBJECTS) *~ core*





