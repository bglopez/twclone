#include "server.h"
#include <string.h>
#include <nspr.h>
#include <stdio.h>


//extern void memory_error ();  /*readini.c */
/*
int
insert_ini_switch_options (char *switches, char *portnum, char **prargv,
			   int a)
{
  char *p;
  char *tmp;
  char *tmp_hold;
  char *part_two;

  tmp_hold = (char *)PR_MALLOC(strlen (switches) + 10);
  tmp = switches;

  if ((part_two = strstr (switches, "_PORT_")) != NULL)
    {
      p = strtok (switches, "_PORT_");
      strcpy (tmp_hold, p);
      part_two += 6;
      strcat (tmp_hold, portnum);
      strcat (tmp_hold, part_two);
    }

  switches = tmp_hold;
  p = strtok (switches, " ");
  prargv[a] = p;
  while ((p = strtok (NULL, " ")) != NULL)
    {
      a++;
      prargv[a] = p;
    }
  a++;
  return a;
}
*/


void
print_err_msg (PRErrorCode eTheError)
{

  switch (eTheError)
    {
    case PR_OUT_OF_MEMORY_ERROR:
      printf ("PR_GetError is PR_OUT_OF_MEMORY_ERROR\n");
      break;
    case PR_BAD_DESCRIPTOR_ERROR:
      printf ("PR_GetError is PR_BAD_DESCRIPTOR_ERROR\n");
      break;
    case PR_WOULD_BLOCK_ERROR:
      printf ("PR_GetError is PR_WOULD_BLOCK_ERROR\n");
      break;
    case PR_ACCESS_FAULT_ERROR:
      printf ("PR_GetError is PR_ACCESS_FAULT_ERROR\n");
      break;
    case PR_INVALID_METHOD_ERROR:
      printf ("PR_GetError is PR_INVALID_METHOD_ERROR \n");
      break;
    case PR_ILLEGAL_ACCESS_ERROR:
      printf ("PR_GetError is PR_ILLEGAL_ACCESS_ERROR\n");
      break;
    case PR_UNKNOWN_ERROR:
      printf ("PR_GetError is PR_UNKNOWN_ERROR \n");
      break;
    case PR_PENDING_INTERRUPT_ERROR:
      printf ("PR_GetError is PR_PENDING_INTERRUPT_ERROR\n");
      break;
    case PR_NOT_IMPLEMENTED_ERROR:
      printf ("PR_GetError is PR_NOT_IMPLEMENTED_ERROR \n");
      break;
    case PR_IO_ERROR:
      printf ("PR_GetError is PR_IO_ERROR \n");
      break;
    case PR_IO_TIMEOUT_ERROR:
      printf ("PR_GetError is PR_IO_TIMEOUT_ERROR \n");
      break;
    case PR_IO_PENDING_ERROR:
      printf ("PR_GetError is PR_IO_PENDING_ERROR\n");
      break;
    case PR_DIRECTORY_OPEN_ERROR:
      printf ("PR_GetError is PR_DIRECTORY_OPEN_ERROR  \n");
      break;
    case PR_INVALID_ARGUMENT_ERROR:
      printf ("PR_GetError is PR_INVALID_ARGUMENT_ERROR\n");
      break;
    case PR_ADDRESS_NOT_AVAILABLE_ERROR:
      printf ("PR_GetError is PR_ADDRESS_NOT_AVAILABLE_ERROR \n");
      break;
    case PR_ADDRESS_NOT_SUPPORTED_ERROR:
      printf ("PR_GetError is  PR_ADDRESS_NOT_SUPPORTED_ERROR\n");
      break;
    case PR_IS_CONNECTED_ERROR:
      printf ("PR_GetError is  PR_IS_CONNECTED_ERROR\n");
      break;
    case PR_BAD_ADDRESS_ERROR:
      printf ("PR_GetError is  PR_BAD_ADDRESS_ERROR\n");
      break;
    case PR_ADDRESS_IN_USE_ERROR:
      printf ("PR_GetError is  PR_ADDRESS_IN_USE_ERROR\n");
      break;
    case PR_CONNECT_REFUSED_ERROR:
      printf ("PR_GetError is PR_CONNECT_REFUSED_ERROR\n");
      break;
    case PR_NETWORK_UNREACHABLE_ERROR:
      printf ("PR_GetError is PR_NETWORK_UNREACHABLE_ERROR\n");
      break;
    case PR_CONNECT_TIMEOUT_ERROR:
      printf ("PR_GetError is  PR_CONNECT_TIMEOUT_ERROR\n");
      break;
    case PR_NOT_CONNECTED_ERROR:
      printf ("PR_GetError is PR_CONNECT_TIMEOUT_ERROR\n");
      break;
    case PR_LOAD_LIBRARY_ERROR:
      printf ("PR_GetError is PR_LOAD_LIBRARY_ERRO\n");
      break;
    case PR_UNLOAD_LIBRARY_ERROR:
      printf ("PR_GetError is PR_UNLOAD_LIBRARY_ERROR\n");
      break;
    case PR_FIND_SYMBOL_ERROR:
      printf ("PR_GetError is PR_FIND_SYMBOL_ERROR\n");
      break;
    case PR_INSUFFICIENT_RESOURCES_ERROR:
      printf ("PR_GetError is PR_INSUFFICIENT_RESOURCES_ERROR\n");
      break;
    case PR_DIRECTORY_LOOKUP_ERROR:
      printf ("PR_GetError is  PR_DIRECTORY_LOOKUP_ERROR\n");
      break;
    case PR_TPD_RANGE_ERROR:
      printf ("PR_GetError is PR_TPD_RANGE_ERROR\n");
      break;
    case PR_PROC_DESC_TABLE_FULL_ERROR:
      printf ("PR_GetError is  PR_PROC_DESC_TABLE_FULL_ERROR\n");
      break;
    case PR_SYS_DESC_TABLE_FULL_ERROR:
      printf ("PR_GetError is PR_SYS_DESC_TABLE_FULL_ERROR \n");
      break;
    case PR_NOT_SOCKET_ERROR:
      printf ("PR_GetError is  PR_NOT_SOCKET_ERROR \n");
      break;
    case PR_NOT_TCP_SOCKET_ERROR:
      printf ("PR_GetError is PR_NOT_TCP_SOCKET_ERROR\n");
      break;
    case PR_SOCKET_ADDRESS_IS_BOUND_ERROR:
      printf ("PR_GetError is PR_SOCKET_ADDRESS_IS_BOUND_ERROR\n");
      break;
    case PR_NO_ACCESS_RIGHTS_ERROR:
      printf ("PR_GetError is PR_NO_ACCESS_RIGHTS_ERROR\n");
      break;
    case PR_OPERATION_NOT_SUPPORTED_ERROR:
      printf ("PR_GetError is PR_OPERATION_NOT_SUPPORTED_ERROR\n");
      break;
    case PR_PROTOCOL_NOT_SUPPORTED_ERROR:
      printf ("PR_GetError is PR_PROTOCOL_NOT_SUPPORTED_ERROR\n");
      break;
    case PR_REMOTE_FILE_ERROR:
      printf ("PR_GetError is PR_REMOTE_FILE_ERROR\n");
      break;
    case PR_BUFFER_OVERFLOW_ERROR:
      printf ("PR_GetError is PR_BUFFER_OVERFLOW_ERROR\n");
      break;
    case PR_CONNECT_RESET_ERROR:
      printf ("PR_GetError is PR_CONNECT_RESET_ERROR\n");
      break;
    case PR_RANGE_ERROR:
      printf ("PR_GetError is PR_RANGE_ERROR\n");
      break;
    case PR_DEADLOCK_ERROR:
      printf ("PR_GetError is PR_DEADLOCK_ERROR\n");
      break;
    case PR_FILE_IS_LOCKED_ERROR:
      printf ("PR_GetError is PR_FILE_IS_LOCKED_ERROR\n");
      break;
    case PR_FILE_TOO_BIG_ERROR:
      printf ("PR_GetError is PR_FILE_TOO_BIG_ERROR\n");
      break;
    case PR_NO_DEVICE_SPACE_ERROR:
      printf ("PR_GetError is PR_NO_DEVICE_SPACE_ERROR\n");
      break;
    case PR_PIPE_ERROR:
      printf ("PR_GetError is PR_PIPE_ERROR\n");
      break;
    case PR_NO_SEEK_DEVICE_ERROR:
      printf ("PR_GetError is PR_NO_SEEK_DEVICE_ERROR\n");
      break;
    case PR_IS_DIRECTORY_ERROR:
      printf ("PR_GetError is PR_IS_DIRECTORY_ERROR\n");
      break;
    case PR_LOOP_ERROR:
      printf ("PR_GetError is PR_LOOP_ERROR\n");
      break;
    case PR_NAME_TOO_LONG_ERROR:
      printf ("PR_GetError is PR_NAME_TOO_LONG_ERROR\n");
      break;
    case PR_FILE_NOT_FOUND_ERROR:
      printf ("PR_GetError is PR_FILE_NOT_FOUND_ERROR\n");
      break;
    case PR_NOT_DIRECTORY_ERROR:
      printf ("PR_GetError is PR_NOT_DIRECTORY_ERROR\n");
      break;
    case PR_READ_ONLY_FILESYSTEM_ERROR:
      printf ("PR_GetError is PR_READ_ONLY_FILESYSTEM_ERROR\n");
      break;
    case PR_DIRECTORY_NOT_EMPTY_ERROR:
      printf ("PR_GetError is PR_DIRECTORY_NOT_EMPTY_ERROR\n");
      break;
    case PR_FILESYSTEM_MOUNTED_ERROR:
      printf ("PR_GetError is PR_FILESYSTEM_MOUNTED_ERROR\n");
      break;
    case PR_NOT_SAME_DEVICE_ERROR:
      printf ("PR_GetError is PR_NOT_SAME_DEVICE_ERROR\n");
      break;
    case PR_DIRECTORY_CORRUPTED_ERROR:
      printf ("PR_GetError is PR_DIRECTORY_CORRUPTED_ERROR\n");
      break;
    case PR_FILE_EXISTS_ERROR:
      printf ("PR_GetError is PR_FILE_EXISTS_ERROR\n");
      break;
    case PR_MAX_DIRECTORY_ENTRIES_ERROR:
      printf ("PR_GetError is PR_MAX_DIRECTORY_ENTRIES_ERROR\n");
      break;
    case PR_INVALID_DEVICE_STATE_ERROR:
      printf ("PR_GetError is PR_INVALID_DEVICE_STATE_ERROR\n");
      break;
    case PR_DEVICE_IS_LOCKED_ERROR:
      printf ("PR_GetError is PR_DEVICE_IS_LOCKED_ERROR\n");
      break;
    case PR_NO_MORE_FILES_ERROR:
      printf ("PR_GetError is PR_NO_MORE_FILES_ERROR\n");
      break;
    case PR_END_OF_FILE_ERROR:
      printf ("PR_GetError is PR_END_OF_FILE_ERROR\n");
      break;
    case PR_FILE_SEEK_ERROR:
      printf ("PR_GetError is PR_FILE_SEEK_ERROR\n");
      break;
    case PR_FILE_IS_BUSY_ERROR:
      printf ("PR_GetError is PR_FILE_IS_BUSY_ERROR\n");
      break;
    case PR_IN_PROGRESS_ERROR:
      printf ("PR_GetError is PR_IN_PROGRESS_ERROR\n");
      break;
    case PR_ALREADY_INITIATED_ERROR:
      printf ("PR_GetError is PR_ALREADY_INITIATED_ERROR \n");
      break;
    case PR_GROUP_EMPTY_ERROR:
      printf ("PR_GetError is  PR_GROUP_EMPTY_ERROR\n");
      break;
    case PR_INVALID_STATE_ERROR:
      printf ("PR_GetError is PR_INVALID_STATE_ERROR \n");
      break;
    case PR_MAX_ERROR:
      printf ("PR_GetError is MAX_ERROR \n");
      break;
    default:
      //      printf ("DEFAULT ERROR: %ld \n", eTheError);
      break;
    }

}
