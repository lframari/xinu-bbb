/* yield.c - yield */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  yield  -  Voluntarily relinquish the CPU (end a timeslice)
 *------------------------------------------------------------------------
 */
syscall	yield(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
  
#ifdef LF_FCFS 
  /* ID of our process	*/
  pid32		pid;	
  
  /* the pointer to our process */
  register struct procent *prptr;
#endif

	mask = disable();
  
#ifdef LF_FCFS
  /* Get the current process */
  pid = currpid;
  prptr = &proctab[currpid];
  
  /* First come first serve lets put us back on the ready list 
     everyone has the same priority so we will be at the back
     of the ready list with only the null process behind us.
     We are still in the ready state  */
  prptr->prstate = PR_READY;
	insert(pid, readylist, prptr->prprio);
#endif

	resched();
	restore(mask);
	return OK;
}
