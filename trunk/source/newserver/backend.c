#include "backend.h"

#include <stdio.h>
#include <pthread.h>
#ifdef __linux__	
	#include <db1/db.h>		// dbopen (linux is db1/db.h)
#else	#include <db.h>			//	(others use db.h)	
#endif
#include <limits.h>			// dbopen
#include <fcntl.h>			// flags for dbopen
#include <sys/types.h>		// dbopen
#include <sys/stat.h>		// flags for dbopen
#include <signal.h>

#include "universe.h"		

void* backend_thread(void *arg);
int open_universe(void);

void init_backend_thread(pthread_t *tid){
	fprintf(stderr, "starting backend thread...\n");
	pthread_create(tid, NULL, backend_thread, NULL);
}

void* backend_thread(void *arg){
	DB *univ_db;
	DBT key, data, data2;
	struct sector foo;
	short done=0;
	short yourmom;
	sigset_t newmask;

	sigaddset(&newmask, SIGINT);
	pthread_sigmask(SIG_BLOCK, &newmask, NULL);

	univ_db=dbopen("universe.foo", (O_RDWR | O_CREAT), S_IRWXU, DB_HASH, NULL);
	foo.number = 69;
	foo.nebulae=(char*)malloc(sizeof(char)*6);
	memcpy(foo.nebulae, "hello\0", 6);

	printf("%s is the sector's name\n", foo.nebulae);
	key.data=(void*)malloc(sizeof(int));
	key.size=sizeof(int);
	*(int*)key.data=foo.number;
	data.data=(void*)malloc(sizeof(struct sector));
	data.size=sizeof(struct sector);
	memcpy(data.data, &foo, sizeof(struct sector));

//yourmom=univ_db->get(univ_db, &key, &data2, 0); 
//printf("get returns %d\n", yourmom );
//univ_db->put(univ_db, &key, &data, 0);

printf("get returns %d\n",	univ_db->get(univ_db, &key, &data2, 0) );
//perror("get: ");

	printf("we have here sector %d with message %s\n",
		((struct sector*)data2.data)->number,
		((struct sector*)data2.data)->nebulae);
	//univ_db->sync(univ_db, 0);

	while(!done) {
	}
}
