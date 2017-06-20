/* lfsinfo.c - lfsinfo */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lfsinfo - Get the list of open files on the given master device
 *------------------------------------------------------------------------
 */
syscall	lfsinfo (
	 uint32  *numentryptr,     /* Number of entries */
	 struct lfsdirentry direntries[LF_NUM_DIR_ENT] /* Actual directory entries */
	)
{
	struct	lfdir	*dirptr;	/* Ptr to in-memory directory	*/
  uint32 i;         /* Looping index */
	int32	retval;			/* Value returned from function	*/

  /* make sure we have a good number of entry pointer */
  if (NULL == numentryptr) {
    return SYSERR;
  }

	/* Obtain copy of directory if not already present in memory	*/
	dirptr = &Lf_data.lf_dir;
	wait(Lf_data.lf_mutex);
	if (! Lf_data.lf_dirpresent) {
	  retval = read(Lf_data.lf_dskdev, (char *)dirptr, LF_AREA_DIR);
	  if (retval == SYSERR ) {
      signal(Lf_data.lf_mutex);
      return SYSERR;
	  }
	  if (lfscheck(dirptr) == SYSERR ) {
      kprintf("Disk does not contain a Xinu file system\n");
      signal(Lf_data.lf_mutex);
      return SYSERR;
    }
	  Lf_data.lf_dirpresent = TRUE;
	}

	/* Get the count */
  *numentryptr = dirptr->lfd_nfiles;

  /* Copy the data */
	for (i=0; i< dirptr->lfd_nfiles; ++i) {
    
    /* start with the file name */
		strncpy (direntries[i].filename, dirptr->lfd_files[i].ld_name, LF_NAME_LEN);
    direntries[i].filename[LF_NAME_LEN - 1] = NULLCH;
		
    /* now the size of the file */
    direntries[i].filesize = dirptr->lfd_files[i].ld_size;
	}

  /* Let others have at it */
  signal(Lf_data.lf_mutex);
  
	return OK;
}
