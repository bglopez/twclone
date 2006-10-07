/*
Copyright (C) 2000 Jason C. Garcowski(jcg5@po.cwru.edu), 
                   Ryan Glasnapp(rglasnap@nmt.edu)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifdef __FreeBSD__
#include <sys/time.h>
#endif

#include <pthread.h>
#include <time.h>
#include <errno.h>
#include "msgqueue.h"

int
init_msgqueue ()
{
  int msgid;

  if ((msgid = msgget (rand (), IPC_CREAT | IPC_EXCL | 0600)) < 0)
    {
      perror ("Failure to initialize message queue: ");
      exit (-1);
    }

  return msgid;
}

long
getmsg (int msgid, char *buffer, long mtype)
{
  int len, senderid;
  void *msg = malloc (sizeof (struct msgbuffer));

  //fprintf(stderr, "getmsg: thread %d is attempting to retrieve messages to %d\n", 
  //      pthread_self(), mtype);
#ifdef __FreeBSD__
  struct timespec ts;

  ts.tv_sec=0;
  ts.tv_nsec=100;
  while((len=msgrcv (msgid, msg, sizeof (struct msgcommand), mtype, MSG_NOERROR|IPC_NOWAIT))==-1 && errno==ENOMSG) {
    nanosleep(&ts,NULL);
  }
#else
  len=msgrcv (msgid, msg, sizeof (struct msgcommand), mtype, MSG_NOERROR | IPC_NOWAIT);
#endif

  if (len < 0)
    {
      perror ("getmsg: Couldn't recieve message from the queue: ");
		if (errno == ENOMSG)
			return(-2);
		else
				  ;
			//exit(-1);
    }

  strncpy (buffer, ((struct msgbuffer *) msg)->buffer, BUFF_SIZE);
  senderid = ((struct msgbuffer *) msg)->senderid;


  if (len < BUFF_SIZE)
    buffer[len] = '\0';

//    fprintf(stderr, "getmsg: message '%s' was recieved heading to %d from %d\n", 
//    buffer, mtype, senderid);

  free (msg);

  return senderid;
}

void clean_msgqueues(int msgidin, int msgidout, char *filename)
{
	FILE *msglock=NULL;
	char buffer[BUFF_SIZE];
	int oldmsg;
	struct msqid_ds oldbuffer;

	printf("\n");
	msglock = fopen(filename, "r");
	if (msglock != NULL)
	{
		do
		{
			buffer[0] = '\0';
			fgets(buffer, BUFF_SIZE, msglock);
			oldmsg = popint(buffer, ":");
			if (oldmsg!=0)
			{
 				if(msgctl(oldmsg, IPC_RMID, NULL)==-1)
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
 					printf("Please run 'ipcrm -q %d'\n", oldmsg);
#else
					printf("Please run 'ipcrm msg %d'\n", oldmsg);
#endif
			}
		}while(oldmsg!=0);
		fclose(msglock);
	}
	msglock = fopen(filename, "w");
	fprintf(msglock,"%d:\n", msgidin);
	fprintf(msglock,"%d:\n", msgidout);
	fclose(msglock);
	return;
}

void sendmesg (int msgid, char *buffer, long mtype)
{
  void *msg = malloc (sizeof (struct msgbuffer));

  strncpy (((struct msgbuffer *) msg)->buffer, buffer, BUFF_SIZE);
  ((struct msgbuffer *) msg)->mtype = mtype;
  ((struct msgbuffer *) msg)->senderid = mtype;

//  fprintf(stderr, "sendmsg: Sending message '%s' from %d to %d\n", 
//  buffer, mtype, mtype);

  if (msgsnd (msgid, msg, sizeof (struct msgbuffer), 0) < 0)
    {
      perror ("sendmesg: unable to send message to queue: ");
      exit (-1);
    }

  free (msg);
  return;
}

long
getdata (int msgid, struct msgcommand *data, long mtype)
{
  int len, senderid;
#ifdef __FreeBSD__
  struct timespec ts;

  ts.tv_sec=0;
  ts.tv_nsec=100;
  while((len=msgrcv (msgid, (void *) data, sizeof (struct msgcommand), mtype, MSG_NOERROR|IPC_NOWAIT))==-1 && errno==ENOMSG) 
  {
    nanosleep(&ts,NULL);
  }
#else
   len=msgrcv (msgid, (void *) data, sizeof (struct msgcommand), mtype, MSG_NOERROR | IPC_NOWAIT);
#endif
  
   if (len  < 0)
	{
      if (errno == ENOMSG)
		{
			return(-2);
		}
		else
      	perror ("getdata: Couldn't recieve message from the queue: ");
			//exit (-1);
   }

  senderid = (struct msgbuf *)data->mtype;

  return senderid;
}

void
senddata (int msgid, struct msgcommand *data, long mtype)
{
  data->mtype = mtype;
  data->senderid = mtype;

  if (msgsnd (msgid, data, sizeof (struct msgcommand), 0) < 0)
    {
      perror ("senddata: unable to send message to queue: ");
      exit (-1);
    }

  return;
}
