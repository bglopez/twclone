#ifndef _BACKEND_H_
#define _BACKEND_H_

#include <pthread.h>	// pthread_t

#define TICK_INTERVAL 1000	// time, in ns, to wait between each tick

// This is where background updating should take place
// 	(i.e. updating ports, planets, aliens, et c.)

void init_backend_thread(pthread_t *tid);

#endif
