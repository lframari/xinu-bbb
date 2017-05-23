/* recvtime.c - recvtime */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  recvtime  -  Wait specified time to receive a message on a given
 *               slot and then returns
 *------------------------------------------------------------------------
 */
umsg32	recvtime(
    uint32  slot,   /* The slot number for the message */
	  int32		maxwait		/* Ticks to wait before timeout */
        )
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent	*prptr;		/* Tbl entry of current process	*/
	umsg32	msg;			/* Message to return		*/
  uint32 slot_bit;          /* The bit that represents our slot */

	if (maxwait < 0) {
		return SYSERR;
	}
  
  /* before locking things down, is the slot number valid */
  /* 0 to 31 is valid */
  if (slot >= MAX_MSG_SLOTS) {
    /* Rampant slot tsk tsk */
    return SYSERR;
  }
  
  /* Calculate the slot bit */
  slot_bit = 0x01 << slot;
  
	mask = disable();

	/* Schedule wakeup and place process in timed-receive state */

	prptr = &proctab[currpid];
	/* Is there anything in the slot */
  if (0 == (slot_bit & (prptr->prhasmsg))) {
		if (insertd(currpid,sleepq,maxwait) == SYSERR) {
			restore(mask);
			return SYSERR;
		}
		prptr->prstate = PR_RECTIM;
		resched();
	}

	/* Either message arrived or timer expired */

	/* Is there anything in the slot */
  if (0 != (slot_bit & (prptr->prhasmsg))) {
		msg = prptr->prmsg[slot];	/* Retrieve message		*/
    (prptr->prhasmsg) &= (~slot_bit); /* Reset message flag		*/
	} else {
		msg = TIMEOUT;
	}
	restore(mask);
	return msg;
}
