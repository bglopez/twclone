/*
** Copyright (c) 2002 Rick Dearman <rick@ricken.demon.co.uk>
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** $Author: eryndil $
** $Date: 2002-07-17 14:24:36 $
** $RCSfile: server.c,v $
** $Revision: 1.1.1.1 $
** $State: Exp $
**
** Description: This program is used to listen on ports 
**		described in the ini file and then launch
**              the correct game based on that information.
**
*/


/*******************************
 ***  Includes
 ******************************/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <nspr.h>
#include "server.h"
#include <prmem.h>


extern struct serverdesc thisserver;
char *MyIPAddress;

/*******************************
 ** External Function declarations
 ******************************/

extern void server ();		/*networks.c */


int
main (int argc, char **argv)
{

  if (argc != 3)
    {
      printf ("Usage: %s <IP address> <port>\n", argv[0]);
      return -1;
    }
  MyIPAddress = (char *) PR_MALLOC (strlen (argv[1]) + 1);
  strcpy (MyIPAddress, argv[1]);
  thisserver.listen_port = atoi (argv[2]);
  server ();
  return 0;
}
