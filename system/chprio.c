/* chprio.c - chprio */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  chprio  -  Change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
pri16	chprio(
	  pid32		pid,		/* ID of process to change	*/
	  pri16		newprio		/* New priority			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	oldprio;		/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return (pri16) SYSERR;
	}
	prptr = &proctab[pid];
	oldprio = prptr->prprio;
	prptr->prprio = newprio;
  
#ifdef LF_MALLEABLE_PRIORITY
  /* Make sure we are still in the proper priority range if malleable */
  if (0 != prptr->malleable) {
    if (MIN_MALLEABLE_PRIORITY > prptr->prprio) {
      /* Too low cap the minimum */
      prptr->prprio = MIN_MALLEABLE_PRIORITY;
    }
    else if (MAX_MALLEABLE_PRIORITY < prptr->prprio) {
      /* Too high cap the maximum */
      prptr->prprio = MAX_MALLEABLE_PRIORITY;
    }
  }
#endif
	restore(mask);
	return oldprio;
}
