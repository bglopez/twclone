#include "console.h"
#include <stdio.h>

void init_console(void){
	int done=0;

	fprintf(stderr, "starting sysop console...\n");
	while(!done){
		fprintf(stdout, "cmd> ");
		switch(getchar()){
		case 'q':
		case 'Q': done=1; break;
		default: break;
		}
	}
	fprintf(stdout, "console exiting....\n");
}
