/* clkhandler.c - clkhandler */

#include <xinu.h>

/*-----------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *-----------------------------------------------------------------------
 */
void	clkhandler()
{
#ifdef LF_MALLEABLE_PRIORITY
  
  struct	procent	*prptr;		/* pointer to process		*/
  
  /* flag to indicate if we need to evaluate processes */
  uint32 eval_procs = 0;
  
  /* Our current process ID */
  uint32 pid;
  
  /* The old priority */
  pri16	prprio_old;
  
  char *pstate[]	= {		/* names for process states	*/
		"free ", "curr ", "ready", "recv ", "sleep", "susp ",
		"wait ", "rtime"};
 
#endif
	
	volatile struct am335x_timer1ms *csrptr =
			(struct am335x_timer1ms *)0x44E31000;
			/* Set csrptr to address of timer CSR	    */

	/* If there is no interrupt, return */

	if((csrptr->tisr & AM335X_TIMER1MS_TISR_OVF_IT_FLAG) == 0) {
		return;
	}

	/* Acknowledge the interrupt */

	csrptr->tisr = AM335X_TIMER1MS_TISR_OVF_IT_FLAG;

	/* Increment 1000ms counter */

	count1000++;

	/* After 1 sec, increment clktime */

	if(count1000 >= 1000) {
		clktime++;
		count1000 = 0;
	}
  
#ifdef LF_MALLEABLE_PRIORITY
  /* Lets see if we are malleable get the current process */
  pid = currpid;
  prptr = &proctab[currpid];
  
  /* Make sure we are not skipping this one first time */
  if (CPU_USAGE_IGNORE != prptr->cpu_usage)
  {
    /* Bump the usage */
    ++prptr->cpu_usage;
  }
#endif

	/* check if sleep queue is empty */

	if(!isempty(sleepq)) {

		/* sleepq nonempty, decrement the key of */
		/* topmost process on sleepq		 */

		if((--queuetab[firstid(sleepq)].qkey) == 0) {

			wakeup();
		}
	}
 
#ifdef LF_MALLEABLE_PRIORITY
  if (0 == (--cpu_eval_countdown))
  {
    /* time to reevaluate */
    eval_procs = 1;
    
    /* Reset the evaluation time */
    cpu_eval_countdown = CPU_EVAL_QUANTUM; 
  }
#endif 

	/* Decrement the preemption counter */
	/* Reschedule if necessary	    */

	if((--preempt) == 0) {
		preempt = QUANTUM;

#ifndef LF_MALLEABLE_PRIORITY    
    resched();
#else      
    if (0 == eval_procs) {
      /* Do not bother to reschedule if we are reevaluating */
      resched();
    }
#endif
	}

#ifdef LF_MALLEABLE_PRIORITY  
  if (0 != eval_procs) {
    
    /* Are we displaying stuff */
    if (0 != display_sched_stats) {
      
      /* Print the headers */
      kprintf("\n\nLW Mark=%d HW Mark=%d\n", lw_cpu_mark, hw_cpu_mark);
      kprintf("%3s %-16s %5s %4s %9s %9s\n",
         "Pid", "Name", "State", "Prio", "Malleable", "CPU Usage");
      kprintf("%3s %-16s %5s %4s %9s %9s\n",
           "---", "----------------", "-----", "----",
           "---------", "---------");
    }
         
    for (pid = 0; pid < NPROC; pid++) {
		  prptr = &proctab[pid];
		  if (prptr->prstate != PR_FREE) {  /* skip unused slots	*/
        
        /* See if we are malleable */
        if (0 != prptr->malleable) {
          
          /* get the old priority */
          prprio_old = prptr->prprio;
          
          if (hw_cpu_mark <= prptr->cpu_usage) {
            /* Massive CPU usage lets knock it down */
            prptr->prprio /= MALLEABLE_PRIORITY_REDUCTION_FACTOR;
          }
          else if (lw_cpu_mark >= prptr->cpu_usage) {
            /* Starving lets see if we can bump it up */
            prptr->prprio += MALLEABLE_PRIORITY_INCREASE_AMOUNT;
          }
          
          /* Make sure we are still in the proper priority range */
          if (MIN_MALLEABLE_PRIORITY > prptr->prprio) {
            /* Too low cap the minimum */
            prptr->prprio = MIN_MALLEABLE_PRIORITY;
          }
          else if (MAX_MALLEABLE_PRIORITY < prptr->prprio) {
            /* Too high cap the maximum */
            prptr->prprio = MAX_MALLEABLE_PRIORITY;
          }
          
          /* Are we the current */
          if (prptr->prstate == PR_CURR) {
            /* Nothing to do the current resched below will handle that */
          }
          if ((prptr->prstate == PR_READY) && (prprio_old != prptr->prprio)) {
            /* We ready and our priority has changed update our readylist position*/
            getitem(pid);
            
            /* Back into the list */
	          insert(pid, readylist, prptr->prprio);
          }
        }
             
        /* Are we displaying stuff */
        if (0 != display_sched_stats) {
          /* Yes display scheduling stats */
          kprintf("%3d %-16s %s %4d %9d %8d\n",
            pid, prptr->prname, pstate[(int)prptr->prstate],
            prptr->prprio, 
            (int)prptr->malleable, (int)prptr->cpu_usage);
        }
          
        /* Reset the CPU Usage */
        prptr->cpu_usage = 0;
       
      }
		}
    
    /* Do any needed rescheduling */
    resched();
  }
#endif
  

}
