/* xsh_lab04.c - xsh_lab04 */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/* Global variables and defines for HW 06 */


#define HW06_NUM_PROCS  3   /* Number of processes use in Homework 6 */ 
  
 /* The process IDs */ 
pid32 pids[HW06_NUM_PROCS];
  
void producer(char* name) {
  
}



/*------------------------------------------------------------------------
 * xsh_hw06  -  Shell command to test HW 06
 *------------------------------------------------------------------------
 */
shellcmd xsh_hw06(int nargs, char *args[])
{
  /* Looping index */
  int i;
  
  /* Used for process creation */
  int pidIndex; 
  pidIndex = 0;

	/* For argument '--help', emit help about the 'hw06' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tRun HW 06 from CIS 657\n");
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
  

    
    /* Give the philosopher a name */
    //sprintf(philname, "Philosopher %d", i);
    
    /* Create the philosopher and let he/she go */
    //pids[i] = create(philosopher, 1024, 20, philname, 1, i);
    //resume(pids[i]);

  
  /* Wait for our CR */
  getc(CONSOLE);
  
  /* Lets clean up */
  for (i = 0; i < HW06_NUM_PROCS; i++) {
    kill(pids[i]);
  }
  
	return 0;
}



