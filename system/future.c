/* future.c - future_start_proc_func, future_start, future_delete, 
              future_sched, future_cancel, future_wait */

#include <xinu.h>


/*------------------------------------------------------------------------
 *  future_delete  -  deletes a scheduled function entry. Local function
 *                    assumes interrupts are disabled
 *------------------------------------------------------------------------
 */
void future_delete(struct fsent* fsptr) {
   
   /* One less entry */
  --fscount;
  
  /* Delete the semaphore */
  semdelete(fsptr->fssem);
  
  /* Now lets clear out the entry and return it to the list */
  fsptr->fsstate   = FS_FREE;	/* we are now free */
  fsptr->fspsid    = -1;
  fsptr->fsdelayms = 0;
  fsptr->fsfunc    = NULL;
  fsptr->fsnumargs = 0;
  fsptr->fssem     = 0;
  fsptr->fsreturn  = 0;
   
  /* no previous or next yet */
  fsptr->fsnext = NULL;
  fsptr->fsprev = NULL;
  
  if ( NULL == fsfreehead ) {
    /* List is entry */
    fsfreehead = fsptr;
    fsfreetail = fsptr;  
  }
  else {
    /* Linked onto the end */
    fsfreetail->fsnext = fsptr;
    fsptr->fsprev = fsfreetail;
  }
}

/*------------------------------------------------------------------------
 *  future_start  -  Used for process creation for future scheduling.
 *                   Captures the return value
 *------------------------------------------------------------------------
 */ 
void future_start_proc_func(struct fsent* fsptr) {
  
  uint32 fsreturn;     /* The return value for the future sched */
  
  /* Default the value */
  fsreturn = SYSERR;
  
  /* Call the function based on the number of arguments */
  switch (fsptr->fsnumargs)  {
    
    /* Zero args */
    case 0:
      fsreturn = (*fsptr->fsfunc)();
      break;
      
    /* One arg */
    case 1:
      fsreturn = (*fsptr->fsfunc)(fsptr->fsargs[0]);
      break;
      
    /* two args */
    case 2:
      fsreturn = (*fsptr->fsfunc)(fsptr->fsargs[0],
        fsptr->fsargs[1]);
      break;
      
    /* three args */
    case 3:
      fsreturn = (*fsptr->fsfunc)(fsptr->fsargs[0],
        fsptr->fsargs[1], fsptr->fsargs[2]);
      break;
      
    /* four args */
    case 4:
      fsreturn = (*fsptr->fsfunc)(fsptr->fsargs[0],
        fsptr->fsargs[1], fsptr->fsargs[2], fsptr->fsargs[3]);
      break;
      
    /* five args */
    case 5:
      fsreturn = (*fsptr->fsfunc)(fsptr->fsargs[0],
        fsptr->fsargs[1], fsptr->fsargs[2], fsptr->fsargs[3],
        fsptr->fsargs[4]);
        
    /* six args */
    case 6:
      fsreturn = (*fsptr->fsfunc)(fsptr->fsargs[0],
        fsptr->fsargs[1], fsptr->fsargs[2], fsptr->fsargs[3],
        fsptr->fsargs[4], fsptr->fsargs[5]);
        
    /* seven args */
    case 7:
      fsreturn = (*fsptr->fsfunc)(fsptr->fsargs[0],
        fsptr->fsargs[1], fsptr->fsargs[2], fsptr->fsargs[3],
        fsptr->fsargs[4], fsptr->fsargs[5], fsptr->fsargs[6]);
      break;
      
    /* eight args */
    case 8:
      fsreturn = (*fsptr->fsfunc)(fsptr->fsargs[0],
        fsptr->fsargs[1], fsptr->fsargs[2], fsptr->fsargs[3],
        fsptr->fsargs[4], fsptr->fsargs[5], fsptr->fsargs[6],
        fsptr->fsargs[7]);
      break;
  }
  
  /* OK we are done wake up any waiters */
  if (-1 == semcount(fsptr->fssem)) {
    /* Yes set them free */
    fsptr->fsreturn = fsreturn;
    signal(fsptr->fssem);
  }
  else {
    /* No one is waiting we must clean up */
    future_delete(fsptr);
  }
  
}

/*------------------------------------------------------------------------
 *  future_start  -  Starts the execution of a future scheduled function
 *                   Assumes interrupts are disabled
 *------------------------------------------------------------------------
 */
void future_start(struct fsent* fsptr) {
  
  /* Create the new process */
  fsptr->fspsid = create(future_start_proc_func, 
    FSINITSTK, FSINITPRIO, fsptr->fsname, 1, fsptr);
  
  /* Lets let it go */
  resume(fsptr->fspsid);
}

/*------------------------------------------------------------------------
 *  future_processing  -  handles the waiting list for futures. Called
 *                        from the clock handler
 *------------------------------------------------------------------------
 */
void future_processing() {
  
  struct fsent* fsptr; /* Future start pointer */
  
  /* If the waiting queue is not emtpy */
  if (NULL != fswaiting) {

		if((--(fswaiting->fsdelayms)) == 0) {
      
      /* future sched nonempty, decrement the key of */
		  /* topmost process on waiting queue		 */
      resched_cntl(DEFER_START);
      
      /* Start all zero delaysed items */
      while ((NULL != fswaiting) && (0 == fswaiting->fsdelayms)) {
        
        /* Get the current entry */
        fsptr = fswaiting;
        
        /* Move the waiting pointer forward */
        fswaiting = fsptr->fsnext;
        
        /* Unlink it */
        fsptr->fsnext = NULL;
        fsptr->fsprev = NULL;
        
        /* Start it up */
        future_start(fsptr);
      }
      
      /* Let the scheduling begin */
      resched_cntl(DEFER_STOP);
      
		}

	}
}

/*------------------------------------------------------------------------
 *  future_sched  -  schedule a function in the future
 *------------------------------------------------------------------------
 */
fsid32	future_sched(
	  uint32 msdelay,     /* Delay before invoking the function */
    uint32 (*func)(),   /* the function to invoke */
    uint32 numargs,     /* Number of arguments */
    ...                 /* The arguments for the function */
	)
{
	intmask 	mask;    	  /* interrupt mask		*/
  struct fsent* fsptr;  /* The free future schedule entry */
  struct fsent* fsnextptr;  /* The next pointer in the waiting list */
  struct fsent* fsprevptr;  /* The previous pointer in the waiting list */
  fsid32 fsid;              /* FS ID of the new entry */
  uint32* argptr;           /* Pointer to the arguments on our stack */
  uint32 argindex;          /* The argument index */
	
  /* Lock out interrupts */
	mask = disable();
  
  /* Time for some validation */
	if ((0 == msdelay) ||
      (FS_MAX_ARGS < numargs) ||
      (NULL == fsfreehead) ||
      (NULL == func)) {
		restore(mask);
		return SYSERR;
	}

  /* One more in use */
	fscount++;
	fsptr = fsfreehead;
  
  /* Update our queues */
  fsfreehead = fsptr->fsnext;
  if (NULL == fsfreehead) {
    /* Drained the list */
    fsfreetail = NULL;
  }
   
  /* Get our FS ID */
  fsid = fsptr->fsid;

	/* initialize process table entry for new request */
	fsptr->fsstate   = FS_WAIT;	/* initial state is waiting (counting down)	*/
  fsptr->fsdelayms = msdelay;
  fsptr->fsfunc    = func;
  fsptr->fsnumargs = numargs;
  fsptr->fssem     = semcreate(0);  /* Start at zero used to signal our future waiters */
  fsptr->fsreturn  = 0;
  fsptr->fspsid    = -1;
  
  /* form the future sched name */
  sprintf(fsptr->fsname, "FS: %04d", fsid);
  
  /* get our argument pointer one past the number of arguments */
  argptr = &numargs;
  ++argptr;
  
  /* Lets copy the arguments */ 
  for (argindex = 0; argindex < numargs; ++argindex) {
    
    /* Copy the current argument */
    fsptr->fsargs[argindex] = *argptr;
    
    /* Onto the next argument */
    ++argptr;
  }
   
  /* no previous or next yet */
  fsptr->fsnext = NULL;
  fsptr->fsprev = NULL;
  
  /* Now lets insert ourselves into the waiting queue */
	if (NULL == fswaiting) {
    /* looks like we are the first */
    fswaiting = fsptr;
  }
  else {
    
    /* See if we are at the front */
    if (fsptr->fsdelayms < fswaiting->fsdelayms)
    {
      /* We are in the start take our time off of the next */
      fswaiting->fsdelayms -= fsptr->fsdelayms;
      
      /* Link us onto the front of the list */
      fsptr->fsnext = fswaiting;
      fswaiting->fsprev = fsptr;
      fswaiting = fsptr;
    }
    else {
      /* We are not at the start find out where we are */
      fsprevptr = fswaiting;
      fsnextptr = fsprevptr->fsnext;
      fsptr->fsdelayms -= fsprevptr->fsdelayms;
      
      /* Loop until we find the proper time relative location */
      while ((NULL != fsnextptr) && (fsptr->fsdelayms >= fsnextptr->fsdelayms)) {
        
        /* Adjust the time */
        fsptr->fsdelayms -= fsnextptr->fsdelayms;
        
        /* Move to the next one */
        fsprevptr = fsnextptr;
        fsnextptr = fsprevptr->fsnext;
      }
      
      /* Fill in our location */
      fsptr->fsnext = fsnextptr;
      fsptr->fsprev = fsprevptr;
      fsprevptr->fsnext = fsptr;
      
      /* are we at the end */
      if (NULL != fsnextptr) {
        fsnextptr->fsprev = fsptr;
        
        /* Adjust the next entries time */
        fsnextptr->fsdelayms -= fsptr->fsdelayms;      
      }
    }
 
  }

  
  /* Restore interrupts to the previous state */
	restore(mask);
	return fsid;
}

/*------------------------------------------------------------------------
 *  future_cancel  -  cancels a future scheduled function
 *------------------------------------------------------------------------
 */
syscall	future_cancel(fsid32 fsid) {
  
  intmask 	mask;    	  /* interrupt mask		*/
  struct fsent* fsptr;  /* The free future schedule entry */
 
  /* Lock out interrupts */
	mask = disable();
  
  /* Do the validity checking */
  if ( isbadfsid(fsid) ) {
	  restore(mask);
    return SYSERR;
  }
  
  /* Is our FSID waiting */
  fsptr = &(fstab[fsid]);
  
  if (FS_WAIT != fsptr->fsstate) {
    restore(mask);
    return SYSERR;
  }
  
  /* Remove this entry from the list */
  if (fswaiting == fsptr) {
    /* Front of the list this is easy */
    if (NULL == fsptr->fsnext) {
      
      /* The list is now empty */
      fswaiting = NULL;
    }
    else {
      /* Front of the list there is more so adjust */
      fswaiting = fsptr->fsnext;
      
      /* Need to account for the lost time of the entry */
      fswaiting->fsdelayms += fsptr->fsdelayms;
    } 
  }
  else {
    /* Link the pointer previous to us to the one in after us */
    fsptr->fsprev->fsnext = fsptr->fsnext;
    
    /* See if we actually have something in front of us */
    if (NULL != fsptr->fsnext) {
      /* There is someone left */
      fsptr->fsnext->fsprev = fsptr->fsprev;
      
      /* Need to account for the lost time of the next entry */
      fsptr->fsnext->fsdelayms += fsptr->fsdelayms;
    }
  }
  
  /* Clean up our pointers */
  fsptr->fsnext = NULL;
  fsptr->fsprev = NULL;
  
  /* Is someone already waiting on this item */
  if (-1 == semcount(fsptr->fssem)) {
    /* Yes set them free */
    fsptr->fsreturn = FS_TERM;
    signal(fsptr->fssem);
  }
  else {
    /* No one is waiting we must clean up */
    future_delete(fsptr);
  }
  
  /* Restore interrupts to the previous state */
	restore(mask);
  
  /* Things are good */
  return OK;
  
}

/*------------------------------------------------------------------------
 *  future_wait  -  waits on a future scheduled function
 *------------------------------------------------------------------------
 */
syscall	future_wait(fsid32 fsid, uint32* returnval) {
  
  intmask 	mask;    	  /* interrupt mask		*/
  struct fsent* fsptr;  /* The free future schedule entry */ 
	
  /* Lock out interrupts */
	mask = disable();
  
  /* Do the validity checking */
  if ( NULL == returnval ) {
	  restore(mask);
    return SYSERR;
  }
  
  /* Assume an issue */
  *returnval = SYSERR;
  
   /* Do the validity checking */
  if ( isbadfsid(fsid) ) {
	  restore(mask);
    return SYSERR;
  }
  
  /* Is our FSID waiting or executing  */
  fsptr = &(fstab[fsid]);
  
  if ((FS_WAIT != fsptr->fsstate) && (FS_ACTIVE != fsptr->fsstate)) {
    restore(mask);
    return SYSERR;
  }
  
  /* Is someone already waiting on this item */
  if (-1 == semcount(fsptr->fssem)) {
    restore(mask);
    return SYSERR;
  }
  
  /* Lets wait on the semaphore to fire */
  wait(fsptr->fssem);
  
  /* Restore the value */
  *returnval = fsptr->fsreturn;
  
  /* Delete the entry */
  future_delete(fsptr);
  
  /* Restore interrupts to the previous state */
	restore(mask);
  
  /* Things are good */
  return OK;
  
}


