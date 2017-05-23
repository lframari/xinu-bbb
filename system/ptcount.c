/* ptcount.c - ptcount */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptcount  -  Return the count of messages currently waiting in a port
 *              and a tag (0 for no tag). Also, the wildcard queue
 *              can be examined if PT_TAG_WILD_CARD is used for the tag.
 *		  (a non-negative count K means the port contains K
 *		   K messages,  including messages from senders that
 *		   are blocked waiting to send a message; a count of
 *		   negative K means a port is empty and K processes
 *		   are blocked waiting to receive a message)
 *------------------------------------------------------------------------
 */
int32	ptcount(
	  int32		portid,		/* ID of a port to use		*/
    uint16  tag       /* Tag to use or wildcard */
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	count;			/* Count of messages available	*/
	int32	sndcnt;			/* Count of sender semaphore	*/
	struct	ptentry	*ptptr;		/* Pointer to port table entry	*/
  struct pttagdata* ptag;   /* Tag related data */

	mask = disable();
	if ( isbadport(portid) ||
		(ptptr= &porttab[portid])->ptstate != PT_ALLOC ) {
			restore(mask);
			return SYSERR;
	}
  
  /* Initialize count */
  count = 0;

  /* See what is in the wildcard bin */
  if  (PT_TAG_WILD_CARD == tag) {
    
    /* Get count of messages available for untagged */

    count = semcount(ptptr->ptrsem);

    /* If messages are waiting, check for blocked senders */

    if (count >= 0) {
      sndcnt = semcount(ptptr->ptssem);
      if (sndcnt < 0) {	/* -sndcnt senders blocked */
        count += -sndcnt;
      }
    }
  }
  else {
    /* See if we have this tag established for this port */
    ptag = ptfindtag(ptptr, tag);
    
    // Does it already exist */
    if (NULL == ptag) {
      /* Bad tag */
      restore(mask);
      return SYSERR;
    }
    
    /* How many are listed with our tag */
    count = semcount(ptag->ptrsem);

    /* If messages are waiting, check for blocked senders */

    if (count >= 0) {
      sndcnt = semcount(ptag->ptssem);
      if (sndcnt < 0) {	/* -sndcnt senders blocked */
        count += -sndcnt;
      }
    }
  }
	restore(mask);
	return count;
}
