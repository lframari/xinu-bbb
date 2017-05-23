/* recvclr.c - recvclr */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  recvclr  -  Clear incoming message for the given slot, and return 
 *              message if one waiting
 *------------------------------------------------------------------------
 */
umsg32	recvclr(
    uint32  slot    /* The slot number for the message */
  )
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/
  uint32 slot_bit;          /* The bit that represents our slot */
  
  /* before locking things down, is the slot number valid */
  /* 0 to 31 is valid */
  if ((slot >= MAX_MSG_SLOTS) && (MSG_ALL_SLOTS != slot)){
    /* Rampant slot tsk tsk */
    return SYSERR;
  }
  
  /* Calculate the slot bit */
  slot_bit = 0x01 << slot;

	mask = disable();
	prptr = &proctab[currpid];
  
  /* Are we clearing everything */
  if (MSG_ALL_SLOTS != slot) {
  
    /* standard clear, Is there anything in the slot */
    if (0 != (slot_bit & (prptr->prhasmsg))) {
      msg = prptr->prmsg[slot];	      /* Retrieve message		*/
      prptr->prhasmsg &= (~slot_bit); /* Reset message flag		*/
    } else {
      msg = OK;
    }
  }
  else {
    /* All clear */
    prptr->prhasmsg = 0;
    
    /* message is not needed */
    msg = OK;
  }
	restore(mask);
	return msg;
}
