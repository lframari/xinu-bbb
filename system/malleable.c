/* malleable.c - set_malleable, clear_malleable, is_malleable, 
                 set_malleable_levels, get_malleable_levels 
                 set_malleable_verbose, clear_malleable_verbose*/

#include <xinu.h>


#ifdef LF_MALLEABLE_PRIORITY

/* CPU Usage levels */
uint16 lw_cpu_mark;    /* Low Water Mark */
uint16 hw_cpu_mark;    /* High Water Mark */
uint32  display_sched_stats; /* Turns off and on display stats */

/*------------------------------------------------------------------------
 *  set_malleable  -  turns a process into a malleable one in which
 *                    the priorities are adjusted based on CPU usage
 *------------------------------------------------------------------------
 */
syscall set_malleable(pid32 pid)
{
  intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
  
  /* The process entry is good lets make this malleable */
	prptr = &proctab[pid];
  prptr->malleable = 1;
  
  /* Also reset the CPU Usage flag to wait for the next cycle */
  prptr->cpu_usage = CPU_USAGE_IGNORE;
  
  /* Make sure we are still in the proper priority range */
  if (MIN_MALLEABLE_PRIORITY > prptr->prprio) {
    /* Too low cap the minimum */
    prptr->prprio = MIN_MALLEABLE_PRIORITY;
  }
  else if (MAX_MALLEABLE_PRIORITY < prptr->prprio) {
    /* Too high cap the maximum */
    prptr->prprio = MAX_MALLEABLE_PRIORITY;
  }
  
 	restore(mask);
  return OK;
}

/*------------------------------------------------------------------------
 *  clear_malleable  -  turns a process into a non-malleable one in which
 *                      the priorities are adjusted based on CPU usage
 *------------------------------------------------------------------------
 */
syscall clear_malleable(pid32 pid)
{
  intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
  
  /* The process entry is good lets make this not malleable */
	prptr = &proctab[pid];
  prptr->malleable = 0;
  
  /* Also reset the CPU Usage flag to wait for the next cycle */
  prptr->cpu_usage = CPU_USAGE_IGNORE;
  
 	restore(mask);
  return OK;
}

/*------------------------------------------------------------------------
 *  is_malleable  -  turns a process into a non-malleable one in which
 *                      the priorities are adjusted based on CPU usage
 *------------------------------------------------------------------------
 */
syscall is_malleable(pid32 pid, uint16* malleable)
{
  intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
  
  /* The process entry is good lets make this not malleable */
	prptr = &proctab[pid];
  
  /* Gets if the process is malleable */
  *malleable = prptr->malleable;
  
 	restore(mask);
  return OK;
}


/*------------------------------------------------------------------------
 *  set_malleable_levels  -  sets the new malleable low and 
 *                           highwater marks
 *------------------------------------------------------------------------
 */
syscall set_malleable_levels(uint16 new_lw_mark, uint16 new_hw_mark)
{
  intmask	mask;			/* Saved interrupt mask		*/

	/* Validate the low and high water marks are in range and that
     the low water mark is less than the high water mark */
  if ((MIN_MALLEABLE_CPU_MARK > new_lw_mark)
    || (MAX_MALLEABLE_CPU_MARK < new_hw_mark)
    || (new_lw_mark >= new_hw_mark)) {
		return SYSERR;
	}
  
  mask = disable();
 
  /* set the new values for low and high water marks */
  lw_cpu_mark = new_lw_mark;
  hw_cpu_mark = new_hw_mark;
  
 	restore(mask);
  return OK;
}

/*------------------------------------------------------------------------
 *  get_malleable_levels  -  gets the current malleable low and 
 *                           highwater marks
 *------------------------------------------------------------------------
 */
extern syscall get_malleable_levels(uint16* curr_lw_mark, uint16* curr_hw_mark)
{
  intmask	mask;			/* Saved interrupt mask		*/

  mask = disable();
 
  /* get the current values for low and high water marks */
  *curr_lw_mark = lw_cpu_mark;
  *curr_hw_mark = hw_cpu_mark;
  
 	restore(mask);
  return OK;
}

/*------------------------------------------------------------------------
 * set_malleable_verbose  -  turns on verbose reporting for malleable
 *------------------------------------------------------------------------
 */
extern syscall set_malleable_verbose()
{
  intmask	mask;			/* Saved interrupt mask		*/

  mask = disable();
  
  /* Turn on the display stats */
  display_sched_stats = 1;
  
  restore(mask);
  return OK;
}

/*------------------------------------------------------------------------
 * clear_malleable_verbose  -  turns off verbose reporting for malleable
 *------------------------------------------------------------------------
 */
extern syscall clear_malleable_verbose()
{
  intmask	mask;			/* Saved interrupt mask		*/

  mask = disable();
  
  /* Turn off the display stats */
  display_sched_stats = 0;
  
  restore(mask);
  return OK;
}
#endif

