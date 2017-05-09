/* xsh_ctrlmalleable.c - xsh_ctrlmalleable */

#include <xinu.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*------------------------------------------------------------------------
 * xsh_ctrlmalleable  -  Shell command to control malleable properties
 *------------------------------------------------------------------------
 */
shellcmd xsh_ctrlmalleable(int nargs, char *args[])
{
  int lw_mark;
  int hw_mark;
  syscall result;
  pid32 pid;

  
	/* For argument '--help', emit help about the 'xsh_ctrlmalleable' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tRun ctrlmalleable\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
    printf("\t--help\t display this help and exit\n");
		return 0;
	}
  
  if (nargs == 2 && strncmp(args[1], "--verbose", 10) == 0) {
		printf("Going to verbose mode\n");
    set_malleable_verbose();
		return 0;
	}
  
  if (nargs == 2 && strncmp(args[1], "--quiet", 8) == 0) {
		printf("Going to quiet mode\n");
    clear_malleable_verbose();
		return 0;
	}
  
  /* Control levels */
  if (nargs == 4 && strncmp(args[1], "--level", 8) == 0) {
    
    /* Convert the high and low levels  start with the low*/
    lw_mark = atoi(args[2]);
    
    if (0 == lw_mark) {
      fprintf(stderr, "Something wrong with lw mark argument: %s\n", args[0]);
      return 1;
    }
    
    /* Convert the high and low levels  finish with the low*/
    hw_mark = atoi(args[3]);
    
    if (0 == hw_mark) {
      fprintf(stderr, "Something wrong with hw mark argument: %s\n", args[0]);
      return 1;
    }
    
    /* Attempt to set the new levels */
		result = set_malleable_levels((uint16)lw_mark, (uint16)hw_mark);
    if (OK != result) {
      fprintf(stderr, "Something wrong with arguments: %s\n", args[0]);
      return 1;
    }
		return 0;
	}
  
  /* Control malleable pid on */
  if (nargs == 3 && strncmp(args[1], "--pidon", 8) == 0) {
    
    /* Convert the PID */
    pid = atoi(args[2]);
    
    if (0 == pid) {
      fprintf(stderr, "Something wrong with pid argument: %s\n", args[0]);
      return 1;
    }
    
    /* Attempt to turn on malleable */
		result = set_malleable(pid);
    if (OK != result) {
      fprintf(stderr, "Something wrong with arguments: %s\n", args[0]);
      return 1;
    }
		return 0;
  }
  
  /* Control malleable pid off */
  if (nargs == 3 && strncmp(args[1], "--pidoff", 9) == 0) {
    
    /* Convert the PID */
    pid = atoi(args[2]);
    
    if (0 == pid) {
      fprintf(stderr, "Something wrong with pid argument: %s\n", args[0]);
      return 1;
    }
    
    /* Attempt to turn off malleable */
		result = clear_malleable(pid);
    if (OK != result) {
      fprintf(stderr, "Something wrong with arguments: %s\n", args[0]);
      return 1;
    }
		return 0;
  }

  fprintf(stderr, "%s: invalid arguments\n", args[0]);
	fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
	return 1;
 
}
