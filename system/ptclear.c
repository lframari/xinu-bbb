/* ptclear.c - _ptclear */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  _ptclear  -  Used by ptdelete and ptreset to clear or reset a port
 *		   (internal function assumes interrupts disabled and
 *		   arguments have been checked for validity)
 *------------------------------------------------------------------------
 */
void	_ptclear(
	  struct ptentry *ptptr,	/* Table entry to clear		*/
	  uint16       newstate,	/* New state for port		*/
	  int32        (*dispose)(int32, uint16)/* Disposal function to call	*/
	 )
{
	struct	ptnode	*walk;		/* Pointer to walk message list	*/
  struct pttagdata* ptag;   /* Tag related data */
  int tag_index;            /* Index for looping */

	/* Place port in limbo state while waiting processes are freed */

	ptptr->ptstate = PT_LIMBO;
  
  /* Indicate no wild card listeners */
  ptptr->wldcrdlstnr = 0;
  

	ptptr->ptseq++;			/* Reset accession number	*/
	walk = ptptr->pthead;		/* First item on msg list	*/

	if ( walk != NULL ) {		/* If message list nonempty	*/

		/* Walk message list and dispose of each message */

		for( ; walk!=NULL ; walk=walk->ptnext) {
      (*dispose)( walk->ptmsg, walk->pttag );
		}

		/* Link entire message list into the free list */
    (ptptr->pttail)->ptnext = ptfree;
    ptfree = ptptr->pthead;
  }

  // Reset all tag data
  for (tag_index = 0; tag_index < ptptr->ptnumtags; ++tag_index)
  {
    /* Clean up our message list */
    ptag = &(ptptr->pttags[tag_index]);
    
    walk = ptag->pthead;		/* First item on tag msg list	*/

    if ( walk != NULL ) {		/* If message list nonempty	*/

      /* Walk message list and dispose of each message */

      for( ; walk!=NULL ; walk=walk->ptnext) {
        (*dispose)( walk->ptmsg, walk->pttag );
      }
      
      /* Link entire message list into the free list */
      ptag->pttail->ptnext = ptfree;
      ptfree = ptag->pthead;
    }
    
    /* Clear out the list */
    ptag->pttail = ptag->pthead = NULL;
  
    semdelete(ptag->ptssem);
		semdelete(ptag->ptrsem);
  }
  
  /* Back to zero tags */
  ptptr->ptnumtags = 0;
    
	if (newstate == PT_ALLOC) {
		ptptr->pttail = ptptr->pthead = NULL;
		semreset(ptptr->ptssem, ptptr->ptmaxcnt);
		semreset(ptptr->ptrsem, 0);
    
    /* Create the default (yes could be more efficient) */
    ptcreatetag(ptptr, PT_TAG_NONE);
    
    
	} else {
		semdelete(ptptr->ptssem);
		semdelete(ptptr->ptrsem);
	}
	ptptr->ptstate = newstate;
	return;
}
