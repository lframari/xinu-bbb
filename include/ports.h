/* ports.h - isbadport */

#define	NPORTS		30		/* Maximum number of ports	*/
#define	PT_MSGS		100		/* Total messages in system	*/
#define	PT_FREE		1		/* Port is free			*/
#define	PT_LIMBO	2		/* Port is being deleted/reset	*/
#define	PT_ALLOC	3		/* Port is allocated		*/

#define PT_TAG_NONE  0   /* The port tag indicating no tag */
#define PT_TAG_WILD_CARD  ((uint16)0xff)   /* The port tag wildcard */

#define PT_MAX_TAGS       10  /* The maximum number of tags */


struct	ptnode	{			/* Node on list of messages 	*/
	uint32	ptmsg;			/* A one-word message		*/
  uint16  pttag;      /* The tag for the message */
  uint16  ptpad1;     /* Padding for the entry */
  uint32  ptpad2;     /* Padding for the entry */
	struct	ptnode	*ptnext;	/* Pointer to next node on list	*/
};

struct pttagdata {
  uint16  pttagid;    /* The tag ID */
  uint16  ptpad1;     /* Padding for the entry */
  uint32  ptpad2;     /* Padding for the entry */
  sid32	ptssem;		  	/* Sender semaphore		*/
	sid32	ptrsem;		  	/* Receiver semaphore		*/
  struct	ptnode	*pthead;	/* List of message pointers	*/
	struct	ptnode	*pttail;	/* Tail of message list		*/
};

struct	ptentry	{			/* Entry in the port table	*/
	sid32	ptssem;			/* Sender semaphore		*/
	sid32	ptrsem;			/* Receiver semaphore		*/
  uint32 wldcrdlstnr; /* 0 at start or after a reset, indicates if 
                         anyone has signed up for the wildcard */
	uint16	ptstate;		/* Port state (FREE/LIMBO/ALLOC)*/
	uint16	ptmaxcnt;		/* Max messages to be queued	*/
	int32	ptseq;			/* Sequence changed at creation	*/
	struct	ptnode	*pthead;	/* List of message pointers	*/
	struct	ptnode	*pttail;	/* Tail of message list		*/
  uint32 pttagdepth; /* Maximum depth for a given tag */
  uint32  ptnumtags;         /* The number of tags */
  struct pttagdata pttags[PT_MAX_TAGS]; /* The tags for this port */
};

extern	struct	ptnode	*ptfree;	/* List of free nodes		*/
extern	struct	ptentry	porttab[];	/* Port table			*/
extern	int32	ptnextid;		/* Next port ID to try when	*/
					/*   looking for a free slot	*/

#define	isbadport(portid)	( (portid)<0 || (portid)>=NPORTS )

/*------------------------------------------------------------------------
 * ptfindtag  -  Attempts to find the tag in the entry if not returns NULL
 *------------------------------------------------------------------------
 */
extern struct pttagdata* ptfindtag(struct ptentry* pt_entry, uint16  tag);

/*------------------------------------------------------------------------
 * ptcreatetag  -  Attempts to add the tag to the entry if not returns NULL
 *------------------------------------------------------------------------
 */

extern struct pttagdata* ptcreatetag(struct ptentry* pt_entry, uint16  tag);

