/* xsh_xsh_fcfsvalidate.c - xsh_fcfsvalidate */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/* Number of processes created */
#define MAX_MAL_PROC  3

/* The names of our spinning processes */
char *spinning_proc_names[MAX_MAL_PROC] = {
  "Hard Spin",
  "100 ms naps",
  "200 ms naps" 
};

/* The sleeps in our spinning processes */
uint32 spinning_proc_sleep_ms[MAX_MAL_PROC] = {
  0,
  100,
  200
};


/*------------------------------------------------------------------------
 * hard_spinning_proc  -  Process function that will just spin 
 *                        continuously doing a little bit of math
 *                        with a given amount of sleep
 *                   
 *------------------------------------------------------------------------
 */
void spinning_proc(uint32 ms_sleep)
{ 
  while (1) {
    /* Spin for  100 ms */
    MDELAY(100);
    
    if (0 != ms_sleep) {
      /* Take a little slumber */
      sleepms(ms_sleep);
    }
  }
}


/*------------------------------------------------------------------------
 * xsh_priotest  -  Shell command to malleable priority testing
 *------------------------------------------------------------------------
 */
shellcmd xsh_priotest(int nargs, char *args[])
{
  pid32 pids[MAX_MAL_PROC];
  int pid_index;
  int mal_index;
  
	/* For argument '--help', emit help about the 'xsh_priotest' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tRun xsh_priotest\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid number of arguments */
	if (nargs > 1) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}
  
  
  /* Create our processes using the array of information */
  for (pid_index = 0; pid_index < MAX_MAL_PROC; ++pid_index) {
    pids[pid_index] = create(spinning_proc, 1024, 20, 
      spinning_proc_names[pid_index], 1, spinning_proc_sleep_ms[pid_index]);
    resume(pids[pid_index]);
  }
  
  /* Let things go for a bit */
  sleep(5);
  
  printf("Going to malleable\n");
  for (mal_index = 0; mal_index < MAX_MAL_PROC; ++mal_index) {
    set_malleable(pids[mal_index]);
  }
  
	return 0;
}
