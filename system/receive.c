/* receive.c - receive */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  receive  -  Wait for a message on the given slot and return the 
 *              message to the caller
 *------------------------------------------------------------------------
 */
umsg32	receive(
    uint32  slot    /* The slot number for the message */
  )
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/ 
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
  
	prptr = &proctab[currpid];
	/* Is there anything in the slot */
  if (0 == (slot_bit & prptr->prhasmsg)) {
    /* no go into receive wait */
		prptr->prstate = PR_RECV;
		resched();		/* Block until message arrives	*/
	}
  
	msg = prptr->prmsg[slot];		/* Retrieve message		*/
	prptr->prhasmsg &= (~slot_bit); /* Reset message flag		*/
	restore(mask);
	return msg;
}
