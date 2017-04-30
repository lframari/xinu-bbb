/* signaln.c - signaln */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  signaln  -  Signal a semaphore n times, releasing n waiting processes
 *------------------------------------------------------------------------
 */
syscall	signaln(
	  sid32		sem,		/* ID of semaphore to signal	*/
	  int32		count		/* Number of times to signal	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	sentry	*semptr;	/* Ptr to sempahore table entry */
  
#ifdef LF_FCFS 
  /* ID of our process	*/
  pid32		pid;	
  
  /* the pointer to our process */
  register struct procent *prptr;
#endif

	mask = disable();
	if (isbadsem(sem) || (count < 0)) {
		restore(mask);
		return SYSERR;
	}
	semptr = &semtab[sem];
	if (semptr->sstate == S_FREE) {
		restore(mask);
		return SYSERR;
	}

	resched_cntl(DEFER_START);
	for (; count > 0; count--) {
		if ((semptr->scount++) < 0) {
			ready(dequeue(semptr->squeue));
		}
	}
	resched_cntl(DEFER_STOP);
  
#ifdef LF_FCFS
  /* For FCFS we look at a signal as not only handling the semaphore
     but also a yield of the processor */
     
  /* Get the current process */
  pid = currpid;
  prptr = &proctab[currpid];
  
  /* First come first serve lets put us back on the ready list 
     everyone has the same priority so we will be at the back
     of the ready list with only the null process behind us.
     We are still in the ready state  */
  prptr->prstate = PR_READY;
	insert(pid, readylist, prptr->prprio);
  
 	resched();
#endif

	restore(mask);
	return OK;
}
