#include "console.h"
#include "backend.h"
#include <stdio.h>
#include <ctype.h>
#include <signal.h>	// signal handling stuff
#include <unistd.h>	// 	""

void god_mode(void);

void init_console(void){
	short done=0;
	char ch;

	fprintf(stderr, "starting sysop console...\n");
	while(!done){
		fprintf(stdout, "cmd> ");
		ch=getchar();
		switch(tolower(ch)){
		case 'q': done=1; break;
		case '~': god_mode(); break;
		default: 
			printf("unrecognized command %c\n", ch); 
			break;
		}
	}
	fprintf(stdout, "console exiting....\n");
}

void god_mode(void){
	short done=0;
	char ch;

	while(!done){
		fprintf(stdout, "god# ");
		ch=getchar();
		switch(tolower(ch)){
		default: printf("unrecognized command %c.\n", ch); break;
		}
	}
}

