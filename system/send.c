/* send.c - send */

#include <xinu.h>

/*--------------------------------------------------------------------------
 *  send  -  Pass a message to a process/slot and start recipient if waiting
 *--------------------------------------------------------------------------
 */
syscall	send(
	  pid32		pid,		/* ID of recipient process	*/
	  umsg32	msg,		/* Contents of message		*/
    uint32  slot    /* The slot number for the message */
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
  uint32 slot_bit;          /* The bit that represents our slot */

  /* before locking things down, is the slot number valid */
  /* 0 to 31 is valid */
  if (slot >= MAX_MSG_SLOTS) {
    /* Rampant slot tsk tsk */
    return SYSERR;
  }
  
  /* Calculate the slot bit */
  slot_bit = 0x01 << slot;
  
	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	prptr = &proctab[pid];
  /* Is this slot occupied */
	if (0 != (slot_bit & (prptr->prhasmsg))) {
    /* Why yes it is */
		restore(mask);
		return SYSERR;
	}
	prptr->prmsg[slot] = msg;		  /* Deliver message		*/
	prptr->prhasmsg |= slot_bit;	/* Indicate message is waiting	*/

	/* If recipient waiting or in timed-wait make it ready */

	if (prptr->prstate == PR_RECV) {
		ready(pid);
	} else if (prptr->prstate == PR_RECTIM) {
		unsleep(pid);
		ready(pid);
	}
	restore(mask);		/* Restore interrupts */
	return OK;
}
