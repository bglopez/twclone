#include "backend.h"

#include <stdio.h>
#include <pthread.h>
//#include <sys/types.h>		// dbopen
//#include <limits.h>			// dbopen
//#include <db.h>				// dbopen
//#include <fcntl.h>			// flags for dbopen
//#include <sys/stat.h>		// flags for dbopen

#include "universe.h"		

void* backend_thread(void *arg);

void init_backend_thread(pthread_t *tid)
{
	fprintf(stderr, "starting backend thread...\n");
//	pthread_create(tid, NULL, backend_thread, NULL);
}

/*
void* backend_thread(void *arg){
	const DB *univ_db;
	DBT key, data;
	struct sector foo;
	short done=0;

	univ_db=(DB*)dbopen(dbfilename, (O_RDWR | O_CREAT), S_IRWXU, DB_HASH, NULL);

	foo.number = 69;
	foo.nebulae=(char*)malloc(sizeof(char)*6);
	memcpy(foo.nebulae, "hello\0", 6);

	printf("%s is the sector's name\n", foo.nebulae);
	key.data=(void*)malloc(sizeof(int));
	*(int*)key.data=foo.number;
	data.data=(void*)malloc(sizeof(struct sector));
	memcpy(data.data, &foo, sizeof(struct sector));

	univ_db->put(univ_db, &key, &data, 0);

	while(!done) {
	}
}
*/
