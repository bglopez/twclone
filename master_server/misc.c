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
** $RCSfile: misc.c,v $
** $Revision: 1.1.1.1 $
** $State: Exp $
**
** Description: Miscellanious functions used in this program
**
*/

/*******************************
 ***  Includes
 ******************************/
#include <nspr.h>
#include <prmem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"

extern struct serverdesc thisserver;

char *
save_string (char *string)
{
  char *tmp;
  char *new_string;

  if (strlen (string) == 0)
    return string;

  tmp = strtok (string, "\r");
  new_string = (char *) PR_MALLOC (strlen (tmp) + 1);
  strcpy (new_string, tmp);

  /* Get rid of quotes */
  if (new_string[0] == '"')
    new_string++;


  if (new_string[strlen (new_string)] == '"')
    new_string[strlen (new_string) - 1] = '\0';


  return (new_string);
}
