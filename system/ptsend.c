/* ptsend.c - ptsend */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ptfindtag  -  Attempts to find the tag in the entry if not returns NULL
 *------------------------------------------------------------------------
 */
struct pttagdata* ptfindtag(struct ptentry* pt_entry, uint16  tag) {
  
  /* The pointer to the created data */
  struct pttagdata* ptag;
  
  /* The index for looping */
  int32 tag_index;
  
  /* Assume the worse */
  ptag = NULL;
  
  for (tag_index = 0; 
    (tag_index < pt_entry->ptnumtags) && (NULL == ptag); ++tag_index) {
      
    /* Is this the one */
    if (tag == pt_entry->pttags[tag_index].pttagid) {
      /* It is */
      ptag = &(pt_entry->pttags[tag_index]);
    }
    
  }
  
  /* give back what we created if anything */
  return ptag;
  
}

/*------------------------------------------------------------------------
 * ptcreatetag  -  Attempts to add the tag to the entry if not returns NULL
 *------------------------------------------------------------------------
 */

struct pttagdata* ptcreatetag(struct ptentry* pt_entry, uint16  tag) {
  
  /* The pointer to the created data */
  struct pttagdata* ptag;
  
  /* Assume the worse */
  ptag = NULL;
  
  /* do we have more space */
  if (PT_MAX_TAGS > pt_entry->ptnumtags) {
    
    /* Create the new entry */
    ptag = &(pt_entry->pttags[pt_entry->ptnumtags]);
    
    /* Fill in the tag and create the send and receive 
       semaphores as well as the message pointer.*/
    ptag->pttagid = tag;
    ptag->ptssem = semcreate(pt_entry->pttagdepth);
	  ptag->ptrsem = semcreate(0);
    ptag->pthead = ptag->pttail = NULL;
    
    /* Indicate there is one more */
    ++(pt_entry->ptnumtags);
    
  }
  
  /* give back what we created if anything */
  return ptag;
}

/*------------------------------------------------------------------------
 *  ptsend  -  Send a message to a port by adding it to the queue
 *             for the given tag
 *------------------------------------------------------------------------
 */
syscall	ptsend(
	  int32		portid,		/* ID of port to use		*/
    uint16  tag,      /* tag of message */
    umsg32	msg   		/* Message to send		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	ptentry	*ptptr;		/* Pointer to table entry	*/
  struct	pttagdata	*pttag;		/* Pointer to table entry	*/
	int32	seq;			/* Local copy of sequence num.	*/
	struct	ptnode	*msgnode;	/* Allocated message node 	*/
	struct	ptnode	*tailnode;	/* Last node in port or NULL	*/

	mask = disable();
  
  /* Get our pointer */
  ptptr = &(porttab[portid]);
  
	if ( (isbadport(portid)) ||
	     (ptptr->ptstate != PT_ALLOC ) ||
       (tag == PT_TAG_WILD_CARD) ) { /* Cannot send to the wild card */
		restore(mask);
		return SYSERR;
	}
  
  /* Assume the worst */
  pttag = NULL;

	/* Wait for space and verify port has not been reset */

	seq = ptptr->ptseq;		/* Record original sequence	*/
    
  /* See if we have this tag established for this port */
  pttag = ptfindtag(ptptr, tag);
  
  // Does one already exist */
  if (NULL == pttag) {
    /* Now see if we can create a new one */
    pttag = ptcreatetag(ptptr, tag);
  }
  
  /* still do not have a tag entry */
  if (NULL == pttag) {
    /* Things are bad */
    restore(mask);
    return SYSERR;
  }
  
  /* OK lets see how this goes for us */
  if (wait(pttag->ptssem) == SYSERR
      || ptptr->ptstate != PT_ALLOC
      || ptptr->ptseq != seq) {
    restore(mask);
    return SYSERR;
  }
  
	if (ptfree == NULL) {
		panic("Port system ran out of message nodes");
	}

	/* Obtain node from free list by unlinking */

	msgnode = ptfree;		/* Point to first free node	*/
	ptfree  = msgnode->ptnext;	/* Unlink from the free list	*/
	msgnode->ptnext = NULL;		/* Set fields in the node	*/
	msgnode->ptmsg  = msg;
  msgnode->pttag = tag;

	
  /* Put the message into the proper queue for the tag */
  tailnode = pttag->pttail;
  if (tailnode == NULL) {		/* Queue for port was empty	*/
    pttag->pttail = pttag->pthead = msgnode;
  } else {			/* Insert new node at tail	*/
    tailnode->ptnext = msgnode;
    pttag->pttail = msgnode;
  }
  signal(pttag->ptrsem);

  /* Has someone listened on the wildcard channel */
  if (0 != ptptr->wldcrdlstnr) {
    
    /* Yes the have so lets give them the message as well */
    if (ptfree == NULL) {
      panic("Port system ran out of message nodes");
    }

    /* Obtain node from free list by unlinking */

    msgnode = ptfree;		/* Point to first free node	*/
    ptfree  = msgnode->ptnext;	/* Unlink from the free list	*/
    msgnode->ptnext = NULL;		/* Set fields in the node	*/
    msgnode->ptmsg  = msg;
    msgnode->pttag = tag;
   
    /* Link it to the wildcard queue */
    tailnode = ptptr->pttail;
    if (tailnode == NULL) {		/* Queue for port was empty	*/
      ptptr->pttail = ptptr->pthead = msgnode;
    } else {			/* Insert new node at tail	*/
      tailnode->ptnext = msgnode;
      ptptr->pttail = msgnode;
    }
    signal(ptptr->ptrsem);
  }
    
	restore(mask);
	return OK;
}
