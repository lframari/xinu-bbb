/* process.h - isbadpid */

/* Maximum number of processes in the system */

#ifndef NPROC
#define	NPROC		8
#endif		

/* Process state constants */

#define	PR_FREE		0	/* Process table entry is unused	*/
#define	PR_CURR		1	/* Process is currently running		*/
#define	PR_READY	2	/* Process is on ready queue		*/
#define	PR_RECV		3	/* Process waiting for message		*/
#define	PR_SLEEP	4	/* Process is sleeping			*/
#define	PR_SUSP		5	/* Process is suspended			*/
#define	PR_WAIT		6	/* Process is on semaphore queue	*/
#define	PR_RECTIM	7	/* Process is receiving with timeout	*/

/* Miscellaneous process definitions */

#define	PNMLEN		16	/* Length of process "name"		*/
#define	NULLPROC	0	/* ID of the null process		*/

/* Process initialization constants */

#define	INITSTK		65536	/* Initial process stack size		*/
#define	INITPRIO	20	/* Initial process priority		*/
#define	INITRET		userret	/* Address to which process returns	*/

/* Inline code to check process ID (assumes interrupts are disabled)	*/

#define	isbadpid(x)	( ((pid32)(x) < 0) || \
			  ((pid32)(x) >= NPROC) || \
			  (proctab[(x)].prstate == PR_FREE))

/* Number of device descriptors a process can have open */

#define NDESC		5	/* must be odd to make procent 4N bytes	*/

/* Definition of the process table (multiple of 32 bits) */

struct procent {		/* Entry in the process table		*/
	uint16	prstate;	/* Process state: PR_CURR, etc.		*/
	pri16	prprio;		/* Process priority			*/
	char	*prstkptr;	/* Saved stack pointer			*/
	char	*prstkbase;	/* Base of run time stack		*/
	uint32	prstklen;	/* Stack length in bytes		*/
	char	prname[PNMLEN];	/* Process name				*/
	sid32	prsem;		/* Semaphore on which process waits	*/
	pid32	prparent;	/* ID of the creating process		*/
	umsg32	prmsg;		/* Message sent to this process		*/
	bool8	prhasmsg;	/* Nonzero iff msg is valid		*/
	int16	prdesc[NDESC];	/* Device descriptors for process	*/
#ifdef LF_MALLEABLE_PRIORITY
  uint16 malleable; /* Indicates if the process is malleable */
  uint16 cpu_usage; /* For malleable processes this flag indicates the CPU usage*/
#endif
};

/* Marker for the top of a process stack (used to help detect overflow)	*/
#define	STACKMAGIC	0x0A0AAAA9

#ifdef LF_MALLEABLE_PRIORITY
/* CPU Usage levels */
extern uint16 lw_cpu_mark;    /* Low Water Mark */
extern uint16 hw_cpu_mark;    /* High Water Mark */
extern uint32  display_sched_stats;  /* Indicates if CPU scheduling status should be displayed */ 

#define CPU_USAGE_IGNORE  (uint16)0xffff    /* CPU Usage is not is use or waiting for next cycle */
#define MIN_MALLEABLE_PRIORITY  5    /* The minimum malleable priority */
#define MAX_MALLEABLE_PRIORITY  255   /* The maximum malleable priority */

/* The amount we reduce priority by with CPU intensive processes */
#define MALLEABLE_PRIORITY_REDUCTION_FACTOR 2 

/* The amount we reduce priority by with CPU intensive processes */
#define MALLEABLE_PRIORITY_INCREASE_AMOUNT 1 

#define MIN_MALLEABLE_CPU_MARK  0       /* Minimum malleable CPU mark */
#define MAX_MALLEABLE_CPU_MARK  1000    /* Maximum malleable CPU mark */

#define DEFAULT_CPU_LW_MARK  300       /* Default malleable LW CPU mark */
#define DEFAULT_CPU_HW_MARK  400       /* Default malleable HW CPU mark */

#endif

extern	struct	procent proctab[];
extern	int32	prcount;	/* Currently active processes		*/
extern	pid32	currpid;	/* Currently executing process		*/
