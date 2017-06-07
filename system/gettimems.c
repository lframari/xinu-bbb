/* gettimems.c - gettimems */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  gettimems  -  Get xinu time in milliseconds
 *------------------------------------------------------------------------
 */
syscall	gettimems(
	  uint32  *timvarms		/* Location to store the result	*/
	)
{
  intmask	mask;			/* Saved interrupt mask		*/
    
	if (NULL == timvarms) {
    return SYSERR;
  }
  
  mask = disable();
  *timvarms = clktimems;
  restore(mask);
	return OK;
}
