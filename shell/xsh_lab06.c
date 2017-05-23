/* xsh_lab06.c - xsh_lab06 */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/* Global variables for lab 06 */

/* Flag to indicate the receivers should continue */
int keep_receiving;

/* Message number and text size */
#define MSG_NUM_BUFFS 64
#define MSG_TEXT_SIZE 64

/* Number of senders */
#define NSNDRS 5

/* The number of receivers */
#define NRCVRS 6

/* The process IDs */
pid32 pid_sndrs[NSNDRS];
pid32 pid_rcvrs[NRCVRS];

/* The number of ports */
#define NLAB06_PORTS 2

/* Holds the ports */
int32 lab06_ports[NLAB06_PORTS];

/* semaphore to ensure print finishes */
sid32 print_sem;

/*
* Forward Declare the msg_sender, msg_receiver and msg_disposer functions
*/
void msg_sender(int32 sndr_index, int32 port_id, uint16 tag, uint32 sleep_time_ms);
void msg_receiver(int32 rcvr_index, int32 port_id, uint16 tag);
int32 msg_disposer(int32 port_id, uint16 tag);


/*------------------------------------------------------------------------
 * xsh_lab06  -  Shell command to execute lab 06
 *------------------------------------------------------------------------
 */
shellcmd xsh_lab06(int nargs, char *args[])
{
   /* Loop counter */
  int32 i;
  
  /* helpers for building processes */
  int32 sndr_index;
  int32 rcvr_index;
  
  /* diagostics */
  int32 port_index;
  int32 port_num;
  int32 tag;
  int32 msg_count;
  uint32 stack_size = 65536;
  
  /* Have everyone keep receiving */
  keep_receiving = 1;
  
	/* For argument '--help', emit help about the 'lab06' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tRun Lab 06 from CIS 657\n");
    printf("\tIt illustrates using tags with port send and receive.\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid number of arguments */
	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}
  
  /* Lets initialize the port system */
  ptinit(256);

  /* Initialize */
  sndr_index = 0;
  rcvr_index = 0;
  
  /* create the print semaphore */
  print_sem = semcreate(1);
  
  /* Create our two ports */
  lab06_ports[0] = ptcreate(10);
  lab06_ports[1] = ptcreate(10);
  
  printf("Hit carriage return when done\n\n");
  
  /* create our first receiver first port (tag1 = 22, rcvr=0) */
  pid_rcvrs[rcvr_index] = create(msg_receiver, stack_size, 20, "Rcvr0", 3, rcvr_index, lab06_ports[0], 22);
  resume(pid_rcvrs[rcvr_index++]);
  
  /* create two senders first port (tag1, sndr=0) and the second will not have a receiver yet (tag2=33, sndr=1)*/
  pid_sndrs[sndr_index] = create(msg_sender, stack_size, 20, "Sndr0", 4, sndr_index, lab06_ports[0], 22, 1000);
  resume(pid_sndrs[sndr_index++]);
  pid_sndrs[sndr_index] = create(msg_sender, stack_size, 20, "Sndr1", 4, sndr_index, lab06_ports[0], 33, 2000);
  resume(pid_sndrs[sndr_index++]);
  
  /* create a receiver for the first port/second tag (tag2=33, rcvr=1) */
  pid_rcvrs[rcvr_index] = create(msg_receiver, stack_size, 20, "Rcvr1", 3, rcvr_index, lab06_ports[0], 33);
  resume(pid_rcvrs[rcvr_index++]);
  
  /* create a second receiver for the first port/second tag (tag2=33, rcvr=2) */
  pid_rcvrs[rcvr_index] = create(msg_receiver, stack_size, 20, "Rcvr2", 3, rcvr_index, lab06_ports[0], 33);
  resume(pid_rcvrs[rcvr_index++]);
  
  /* create another sender for the first port/first tag (tag1=22, sndr=2) */
  pid_sndrs[sndr_index] = create(msg_sender, stack_size, 20, "Sndr2", 4, sndr_index, lab06_ports[0], 22, 3000);
  resume(pid_sndrs[sndr_index++]);
 
  /* Create a sender (sndr=3) for the second port tag1=22 */
  pid_sndrs[sndr_index] = create(msg_sender, stack_size, 20, "Sndr3", 4, sndr_index, lab06_ports[1], 22, 1000);
  resume(pid_sndrs[sndr_index++]);
  
  /* Create a receiver (rcvr=3) for the second port tag1=22 */
  pid_rcvrs[rcvr_index] = create(msg_receiver, stack_size, 20, "Rcvr3", 3, rcvr_index, lab06_ports[1], 22);
  resume(pid_rcvrs[rcvr_index++]);
  
  /* create a 0 tag (no tag) receiver for the second port rcvr=4 */
  pid_rcvrs[rcvr_index] = create(msg_receiver, stack_size, 20, "Rcvr4", 3, rcvr_index, lab06_ports[1], PT_TAG_NONE);
  resume(pid_rcvrs[rcvr_index++]);
  
  /* create an untagged sender for the second port sndr=4 */
  pid_sndrs[sndr_index] = create(msg_sender, stack_size, 20, "Sndr4", 4, sndr_index, lab06_ports[1], PT_TAG_NONE, 1000);
  resume(pid_sndrs[sndr_index++]); 
  
  /* create a Wildcard tag receiver for the first port rcvr=5 */
  pid_rcvrs[rcvr_index] = create(msg_receiver, stack_size, 20, "Rcvr5", 3, rcvr_index, lab06_ports[0], PT_TAG_WILD_CARD);
  resume(pid_rcvrs[rcvr_index++]);
  
  /* Show what is going out */
  wait(print_sem);
  
  /* Lets look at the current ports we have set up with their tags */
  for (port_index = 0; port_index < NLAB06_PORTS; ++port_index) {
    port_num = lab06_ports[port_index];
    
    tag = PT_TAG_NONE;
    msg_count = ptcount(port_num, tag);
    printf("Port Index: %d Number: %d Tag: %d Count: %d\n", port_index, port_num, tag, msg_count);
    
    tag = 22;
    msg_count = ptcount(port_num, tag);
    printf("Port Index: %d Number: %d Tag: %d Count: %d\n", port_index, port_num, tag, msg_count);
    
    tag = 33;
    msg_count = ptcount(port_num, tag);
    printf("Port Index: %d Number: %d Tag: %d Count: %d\n", port_index, port_num, tag, msg_count);
    
    tag = PT_TAG_WILD_CARD;
    msg_count = ptcount(port_num, tag);
    printf("Port Index: %d Number: %d Tag: %d Count: %d\n", port_index, port_num, tag, msg_count);
  }
  
  /* Let everyone else print now */
  signal(print_sem);
  
  /* Wait for our CR */
  getc(CONSOLE);
  
  /* Time to stop recieving */
  keep_receiving = 0;
   
  /* Give them time */
  sleepms(20000);
   
  /* now clean up the ports should result in the senders ending as well */
  for (i = 0; i < NLAB06_PORTS; i++) {
    ptdelete(lab06_ports[i], msg_disposer);
  }
  
	return 0;
}

/*------------------------------------------------------------------------
 * sender  -  Sends a message to the given port and 
 *------------------------------------------------------------------------
 */

void msg_sender(int32 sndr_index, int32 port_id, 
                uint16 tag, uint32 sleep_time_ms)
{
  /* The results of each send */
  syscall send_results;
  
  /* the message text */
  char msg_text[MSG_NUM_BUFFS][MSG_TEXT_SIZE];
  char *pmsg_text;
  
  /* the message buffer */
  int msg_buff;
  
  /* the message number */
  int msg_num;
  
  /* Initialize the message buffer and count */
  msg_buff = 0;
  msg_num = 0;
  
  /* Things are good so far */
  send_results = OK;
  
  /* Loop until we get an error */
  while (SYSERR != send_results) {
    
    /* Bump the message number */
    ++msg_num;
    
    /* Formulate our message */
    pmsg_text = &(msg_text[msg_buff][0]);
    sprintf(pmsg_text, "Sender=%d port=%d tag=%d msg_num=%d", 
      sndr_index, port_id, tag, msg_num);
    
    /* Show what is going out */
    wait(print_sem);
    printf("Sender=%d port=%d tag=%d sending: %s\n", sndr_index, port_id, tag, pmsg_text);
    signal(print_sem);
      
    /* Send the messge */
    send_results = ptsend(port_id, tag, (uint32)pmsg_text);
    
    /* How did that go */
    if (SYSERR != send_results) {
      /* Sleep this one off since things are good */
      sleepms(sleep_time_ms);
    }
    else {
      /* Bad results */
      wait(print_sem);
      printf("Sender=%d port=%d tag=%d received a system error from ptsend.\n", sndr_index, port_id, tag);
      signal(print_sem);
    }
    
    /* Bump the message buffer */
    ++msg_buff;
    if (MSG_NUM_BUFFS <= msg_buff) {
      /* Make sure we wrap */
      msg_buff = 0;
    }
  }
  
  /* Indicate we are ending */
  wait(print_sem);
  printf("Sender=%d port=%d tag=%d is terminating.\n", sndr_index, port_id, tag);
  signal(print_sem);
}

/*------------------------------------------------------------------------
 * msg_receiver  -  Sits waiting for the given 
 *------------------------------------------------------------------------
 */
void msg_receiver(int32 rcvr_index, int32 port_id, uint16 tag) {
  
  /* the message we are waiting on */
  uint32 msg;
  
  /* Loop until we get are told to stop */
  while (0 != keep_receiving) {
    
    /* get our next message */
    msg = ptrecv(port_id, tag);
    
    /* See if it is good */
    if (SYSERR != msg) {
      /* It is a good message let the world know */
      wait(print_sem);
      printf("Receiver=%d port=%d tag=%d received: %s\n", rcvr_index, port_id, tag, (char*)msg);
      signal(print_sem);
    }
    else {
      /* Bad results */
      wait(print_sem);
      printf("Receiver=%d port=%d tag=%d received a system error from ptrecv.\n", rcvr_index, port_id, tag);
      signal(print_sem);
    }
    
  }
  
  /* Indicate we are ending */
  wait(print_sem);
  printf("Receiver=%d port=%d tag=%d is terminating.\n", rcvr_index, port_id, tag);
  signal(print_sem);
}

/*------------------------------------------------------------------------
 * msg_disposer  -  Called as part of clean up for the given msg and tag
 *------------------------------------------------------------------------
 */
int32 msg_disposer(int32 msg, uint16 tag)
{
  /* Indicate we are ending */
  wait(print_sem);
  printf("Msg msg=%x tag=%d msg text=(%s) is being deleted\n", msg, tag, (char*)msg);
  signal(print_sem);
  
  /* All things are good */
  return OK;
}

