/* xsh_xsh_fcfsvalidate.c - xsh_fcfsvalidate */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/* First mutex */
sid32 fcfs_semid1 = 0;

/* Second mutex */
sid32 fcfs_semid2 = 0;

/*------------------------------------------------------------------------
 * fcfs_proc  -  Function to institute a spinning delay. It takes an
 *               outer loop counter and an innnter loop counter.
 *------------------------------------------------------------------------
 */
void fcfs_delay(uint32 outer_delay,
                uint32 inner_delay)
{
  /* Loop indexes */
  uint32 outer_delay_counter;
  uint32 inner_delay_counter; 
  
  for (outer_delay_counter = 1; outer_delay_counter <= outer_delay; ++outer_delay_counter)
  {
    for (inner_delay_counter = 1; inner_delay_counter <= inner_delay; ++inner_delay_counter)
    {
    
    }
  }
}

/*------------------------------------------------------------------------
 * fcfs_proc  -  Utility function for testing fcfs. Multiple processes
 *               are spawned using the same function. The function
 *               takes an identifying name and a yield flag to indicate
 *               if the process should perfom some yields.
 *------------------------------------------------------------------------
 */
void fcfs_proc(char* name, uint32 yield_flag)
{ 
  /* Print our statement */
  kprintf("%s starting statement\n", name);
 
  /* Simply kill time spinning away */
  fcfs_delay(1, 1000000000);
  
  /* See if we are supposed to yield */
  if (0 != yield_flag)
  {
    kprintf("%s is yielding\n", name);
    yield();
  }
 
   /* Simply kill more time */
  fcfs_delay(1, 1000000000);
  
  /* Wait for the semaphore to be signalled */
  kprintf("%s is about to wait\n", name);
  wait(fcfs_semid1);
  kprintf("%s is done waiting\n", name);
  
  /* Let another one through */
  kprintf("%s is about to signal\n", name);
  signal(fcfs_semid1);
  kprintf("%s is done with the signal\n", name);
}

/*------------------------------------------------------------------------
 * xsh_fcfsvalidate  -  Shell command to execute FCFS Scheduling Validator
 *------------------------------------------------------------------------
 */
shellcmd xsh_fcfsvalidate(int nargs, char *args[])
{
	/* For argument '--help', emit help about the 'xsh_fcfsvalidate' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tRun xsh_fcfsvalidate\n");
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
  
  /* The semaphores starts at zero need to signal before anything starts */
  fcfs_semid1 = semcreate(0);
  fcfs_semid2 = semcreate(0);

  /* Create our three processes the first with the lowest priority */
  resume( create(fcfs_proc, 200, 10, "Proc1", 2, "1st Process (Sans Yield)", 0));
  
  /* In the standard scheduler this one would run with the second highest priority */
  resume( create(fcfs_proc, 200, 40, "Proc2", 2, "2nd Process (With Yield)", 1));
  
  /* In the standard scheduler this one would run with the highest priority */
  resume( create(fcfs_proc, 200, 50, "Proc3", 2, "3rd Process (Sans Yield)", 0));

	/* Lets give the processor up for the kids (child processes) */
  
  kprintf("Pre Signal from xsh_fcfsvalidate\n");
  
  signal(fcfs_semid1);
  
  kprintf("Post Signal from xsh_fcfsvalidate\n");
  
  kprintf("Pre Yield from xsh_fcfsvalidate\n");
  
  yield();
  
  kprintf("Post Yield from xsh_fcfsvalidate\n");
  
	return 0;
}
