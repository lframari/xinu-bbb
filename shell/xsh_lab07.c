/* xsh_lab07.c - xsh_lab07 */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/* Global variables and defines for Lab 07 */

#define LAB_07_NUM_FSIDS  4
fsid32 lab07fsids[LAB_07_NUM_FSIDS];

/*
* Forward Declare the future sched routines 
*/
extern uint32 zeroarg();

extern uint32 onearg(uint32 arg1);

extern uint32 fourarg(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4);

extern uint32 eightarg(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4,
                       uint32 arg5, uint32 arg6, uint32 arg7, uint32 arg8);

/*------------------------------------------------------------------------
 * getclkms  -  Gets the clock time in milliseconds
 *------------------------------------------------------------------------
 */                       
uint32 getclkms() {
  uint32 timems;
  gettimems(&timems);
  return timems;
}            

/*------------------------------------------------------------------------
 * xsh_lab07  -  Shell command to test Lab 07
 *------------------------------------------------------------------------
 */
shellcmd xsh_lab07(int nargs, char *args[]) { 
  uint32 result;
  uint32 fsidindex;
  fsid32 tempfsid;
  uint32 currtime;
  
	/* For argument '--help', emit help about the 'lab07' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tRun Lab 07 from CIS 657\n");
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
 
  /* show the error conditions */
  printf("Error check %s: %d\n", "future_sched(0, zeroarg, 0)", future_sched(0, &zeroarg, 0));
  printf("Error check %s: %d\n", "future_sched(2000, NULL, 0)", future_sched(2000, NULL, 0));
  printf("Error check %s: %d\n", "future_sched(2000, zeroarg, 9)", future_sched(2000, &zeroarg, 9));
  printf("Error check %s: %d\n", "future_cancel(33)", future_cancel(33));
  printf("Error check %s: %d\n", "future_wait(7, result)", future_wait(7, &result));
  printf("Error check %s: %d\n", "future_wait(7 , NULL)", future_wait(7, NULL));
  
  /* Lets schedule one show an error and then kill it */
  tempfsid = future_sched(2000, &onearg, 1, 5);
  printf("Error check %s: %d\n", "future_wait(tempfsid , NULL)", future_wait(tempfsid, NULL));
  
  /* Now kill it */
  future_cancel(tempfsid);
  
  /* sleep a little bit to show it was killed */
  sleepms(3000);
  
  /* Get the start time */
  currtime = getclkms();
  printf("\n\nStart Time = %d\n", currtime); 
  
  /* Lets schedule a series of good ones */
  lab07fsids[2] = future_sched(12000, &zeroarg, 0);
  lab07fsids[0] = future_sched(4000, &eightarg, 8, 1, 2, 3, 4, 5, 6, 7, 8);
  lab07fsids[1] = future_sched(9000, &fourarg, 4, 100, 200, 300, 400);
  lab07fsids[3] = future_sched(19000, &onearg, 1, 10);
    
  /* Now lets wait for our stuff to execute */
  for (fsidindex = 0; fsidindex < LAB_07_NUM_FSIDS; ++fsidindex)  {
  
    future_wait(lab07fsids[fsidindex], &result);
    currtime = getclkms();
    printf("Time = %d - Wait Complete Index = %d result = %d\n", currtime, fsidindex, result);
  }
  
	return 0;
}

/*------------------------------------------------------------------------
 * zeroarg  -  Zero argument future schedule function
 *------------------------------------------------------------------------
 */
uint32 zeroarg() {
  uint32 currtime;
  currtime = getclkms();
  printf("Time = %d - Zero argument () executing\n", currtime);
  return 0;
}

/*------------------------------------------------------------------------
 * onearg  -  One argument future schedule function
 *------------------------------------------------------------------------
 */
uint32 onearg(uint32 arg1) {
  uint32 currtime;
  currtime = getclkms();
  printf("Time = %d - One argument (%d) executing\n", currtime, arg1);
  return 1;
}
  
/*------------------------------------------------------------------------
 * fourarg  -  four argument future schedule function
 *------------------------------------------------------------------------
 */
uint32 fourarg(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4) {
  uint32 currtime;
  currtime = getclkms();
  printf("Time = %d - Four argument (%d, %d, %d, %d) executing\n", 
    currtime, arg1, arg2, arg3, arg4);
  return 4;
}

/*------------------------------------------------------------------------
 * eightarg  -  eight argument future schedule function
 *------------------------------------------------------------------------
 */
uint32 eightarg(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4,
                uint32 arg5, uint32 arg6, uint32 arg7, uint32 arg8) {
  uint32 currtime;
  currtime = getclkms();
  printf("Time = %d - Eight argument (%d, %d, %d, %d, %d, %d, %d, %d) executing\n", 
    currtime, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
  return 8;
}
