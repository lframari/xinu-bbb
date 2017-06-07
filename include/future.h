/* future.h - isbadfsid */

/* Maximum number of future schedulings in the system */

#ifndef NFUTURES
#define	NFUTURES		32
#endif		

/* Process state constants */

#define	FS_FREE		0	/* Future Schedule table entry is unused	*/
#define	FS_WAIT		1	/* Future Schedule is counting down to start		*/
#define	FS_ACTIVE	2	/* Future Schedule is active 		*/
#define	FS_COMP 	3	/* Future Schedule is complete or terminated	*/

#define	FSINITSTK		16384	/* Initial process stack size for a future schedule		*/
#define	FSINITPRIO	120	  /* Initial process priority for a future schedule		*/

#define FS_MAX_ARGS   8   /* Maximum number of arguments */

#define NO_FSID   (-1)          /* Indicates there is no FS ID */
#define FS_TERM   (0xDEADBEEF)   /* Random value to indicate termination */

/* Inline code to check future schedule ID (assumes interrupts are disabled)	*/

#define	isbadfsid(x)	( ((fsid32)(x) < 0) || \
			  ((fsid32)(x) >= NFUTURES) || \
			  (fstab[(x)].fsstate == FS_FREE))



/* Definition of the future schedule table (multiple of 32 bits) */

struct fsent {		/* Entry in the future schedule table		*/
	uint32	fsstate;	/* Future Schedule state: FS_ACTIVE, etc.		*/
  fsid32 fsid;      /* Future schedule ID */
  pid32 fspsid;     /* The process ID of the schedule item */
  uint32 fsdelayms;   /* The delay in milliseconds to start the entity */
  char	fsname[PNMLEN];	/* future sched name (must be proc name length or less)	*/
  uint32(*fsfunc)();   /* the function to be invoked with the future schedule */
  uint32 fsnumargs; /* The number of arguments */
  uint32 fsargs[FS_MAX_ARGS];   /* A pointer to the arguments for the future schedule */
	sid32	fssem;		/* Semaphore on which process waits	for a join */
  uint32 fsreturn;  /* The return value for the future sched */
  struct fsent* fsnext; /* The next Future Schedule Entry */
  struct fsent* fsprev; /* The next Future Schedule Entry */
};

/* The table of FS entries */
extern struct	fsent fstab[];
extern uint32	fscount;	/* Currently in use future schedule entries		*/
extern struct fsent* fsfreehead;  /* The head of the free list of FS entries */
extern struct fsent* fsfreetail;  /* The tail of the free list of FS entries */

extern struct fsent* fswaiting; /* The FS entries that are waiting */
