#ifndef _BACKEND_H_
#define _BACKEND_H_

#include <stdio.h>	// all this stuff is for dbopen and kin
#include <pthread.h>	// pthread_t
#ifdef __linux__
        #include <db1/db.h>             // dbopen (linux is db1/db.h)
#else   #include <db.h>                 //      (others use db.h)
#endif
#include <limits.h>                     // dbopen
#include <fcntl.h>                      // flags for dbopen
#include <sys/types.h>          // dbopen
#include <sys/stat.h>           // flags for dbopen


#define TICK_INTERVAL 1000	// time, in ns, to wait between each tick

// This is where background updating should take place
// 	(i.e. updating ports, planets, aliens, et c.)

void init_backend_thread(pthread_t *tid);

#endif
