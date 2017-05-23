/* xsh_lab04.c - xsh_lab04 */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/* Global variables and defines for HW 06 */

/* Flag to indicate the receivers should continue */
int keep_receiving;

#define HW06_NUM_PROCS  3   /* Number of processes use in Homework 6 */ 
#define HW06_NUM_SLOTS  3   /* Maximum Number of slots used per processes */ 
  
 /* The process IDs */ 
pid32 pids[HW06_NUM_PROCS];

/* The slots allocated to each process */
uint32 num_slots[HW06_NUM_PROCS];
uint32 slots[HW06_NUM_PROCS][HW06_NUM_SLOTS];

/* semaphore to ensure print finishes */
sid32 print_sem;

/*------------------------------------------------------------------------
 * msg_receiver  -  Sits waiting for the given messaage on the given slots
 *------------------------------------------------------------------------
 */
/*
* Forward Declare the msg_receiver
*/
void msg_receiver(int32 rcvr_index, uint32 sleeptimems, bool8 userecvtime,
                  uint32 num_slots, uint32 slot1, uint32 slot2, uint32 slot3) ;

/*------------------------------------------------------------------------
 * xsh_hw06  -  Shell command to test HW 06
 *------------------------------------------------------------------------
 */
shellcmd xsh_hw06(int nargs, char *args[])
{
  /* Looping index */
  int pid_index;
  int slot_index;
  int msg_index;
  
  /* Used for process creation */
  int32 rcvr_index;
  
  /* the base and the increment for our messages */
  umsg32 msg_base;
  umsg32 msg_inc;
  
  /* Validation variables */
  syscall results;
  umsg32 test_msg;
  
  /* Our stack size */
  uint32 stack_size;
  
  /* Initialize our variables */
  msg_base =  0;
  msg_inc  =  100;
  stack_size = 65536;
  rcvr_index = 0;
  
  /* Start them looping */
  keep_receiving = 1;
  

	/* For argument '--help', emit help about the 'hw06' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tRun HW 06 from CIS 657\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid number of arguments */
	if (nargs > 1) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}
 
  /* create the print semaphore */
  print_sem = semcreate(1);
  
  /* Show that the number of slots is checked properly */
  results = send(getpid(), 2233, MAX_MSG_SLOTS);
  if (SYSERR == results) {
    printf("Send validated the slot properly\n");
  }
  else {
    printf("Send validated the slot improperly\n");
  }
  
  test_msg = receive(MAX_MSG_SLOTS);
  if (SYSERR == test_msg) {
    printf("Receiver validated the slot properly\n");
  }
  else {
    printf("Receiver validated the slot improperly\n");
  }
  
  test_msg = recvtime(MAX_MSG_SLOTS, 1000);
  if (SYSERR == test_msg) {
    printf("Recvtime validated the slot properly\n");
  }
  else {
    printf("Recvtime validated the slot improperly\n");
  }
  
  test_msg = recvclr(MAX_MSG_SLOTS);
  if (SYSERR == test_msg) {
    printf("Recvclr validated the slot properly\n");
  }
  else {
    printf("Recvclr validated the slot improperly\n");
  }
  
  printf("\n\n");
  
  /* Lets create our receiver processes start with the standard */
  num_slots[rcvr_index] = 1;
  slots[rcvr_index][0] = 3;
  slots[rcvr_index][1] = 0;
  slots[rcvr_index][2] = 0;
  pids[rcvr_index] = create(msg_receiver, stack_size, 20, "Rcvr0 (receive )", 7, rcvr_index, 0, FALSE,
                            num_slots[rcvr_index], slots[rcvr_index][0],
                            slots[rcvr_index][1], slots[rcvr_index][2]);
  ++rcvr_index;

  /* Lets create our receiver processes next with the recvtime */
  num_slots[rcvr_index] = 1;
  slots[rcvr_index][0] = 2;
  slots[rcvr_index][1] = 0;
  slots[rcvr_index][2] = 0;
  pids[rcvr_index] = create(msg_receiver, stack_size, 20, "Rcvr1 (recvtime)", 7, rcvr_index, 1000, TRUE,
                            num_slots[rcvr_index], slots[rcvr_index][0],
                            slots[rcvr_index][1], slots[rcvr_index][2]);
  ++rcvr_index;
  
  /* Lets create our receiver processes next with the recvclr */
  num_slots[rcvr_index] = 3;
  slots[rcvr_index][0] = 22;
  slots[rcvr_index][1] = 28;
  slots[rcvr_index][2] = 31;
  pids[rcvr_index] = create(msg_receiver, stack_size, 20, "Rcvr2 (recvclr )", 7, rcvr_index, 500, FALSE,
                            num_slots[rcvr_index], slots[rcvr_index][0],
                            slots[rcvr_index][1], slots[rcvr_index][2]);
  ++rcvr_index;

  for (pid_index = 0; pid_index < HW06_NUM_PROCS; ++pid_index) {
    for (slot_index = 0; slot_index < num_slots[pid_index]; ++slot_index) {  
      results = send(pids[pid_index], msg_base + slots[pid_index][slot_index], slots[pid_index][slot_index]);
      if (SYSERR == results) {
        printf("Error sending to receiver=%d slot=%d\n", pid_index, slots[pid_index][slot_index]);
      }
    }
  }  
  
  /* Bump the messages forward */
  msg_base += msg_inc;
  
  /* Lets start up */
  for (pid_index = 0; pid_index < HW06_NUM_PROCS; pid_index++) {
    resume(pids[pid_index]);
  }
  
  /* Now wait a while */
  sleepms(500);
 
  /* lets send messages to each receiver */
  for (msg_index = 0; msg_index < 3; ++msg_index) {
    for (pid_index = 0; pid_index < HW06_NUM_PROCS; ++pid_index) {
      for (slot_index = 0; slot_index < num_slots[pid_index]; ++slot_index) {  
        results = send(pids[pid_index], msg_base + slots[pid_index][slot_index], slots[pid_index][slot_index]);
        if (SYSERR == results) {
          printf("Error sending to receiver=%d slot=%d\n", pid_index, slots[pid_index][slot_index]);
        }
      }
    }
    
    /* Bump the messages forward */
    msg_base += msg_inc;
    
    sleepms(1500);
  }
  
  /* Now wait a while */
  sleepms(200);
  
  /* Lets clean up */
  for (pid_index = 0; pid_index < HW06_NUM_PROCS; pid_index++) {
    kill(pids[pid_index]);
  }
  
  /* Now clean up the print semaphore */
  semdelete(print_sem);
  
	return 0;
}

/*------------------------------------------------------------------------
 * msg_receiver  -  Sits waiting for the given messaage on the given slots
 *                  if userecvtime is set it uses recvtime and will only
 *                  use slot 1, if userecvtime is not set it uses the the 
 *                  recvclr if sleeptimems is not zero and will attempt to 
 *                  read multiple slots if specified. If the sleeptime is
 *                  zero and userecvtime is not set then the standard 
 *                  receive call will be used and look only for slot 1.
 *------------------------------------------------------------------------
 */

void msg_receiver(int32 rcvr_index, uint32 sleeptimems, bool8 userecvtime,
                  uint32 num_slots, uint32 slot1, uint32 slot2, uint32 slot3) {
  
  umsg32 the_msg; /* The message we received */

  /* Now lets clear out some of the messages */
  if (0 == rcvr_index) {
    recvclr(MSG_ALL_SLOTS);
  }
  else if (1 == rcvr_index) {
    recvclr(slots[1][0]);
  }
  else {
    recvclr(slots[2][1]);
  }

  /* Loop until we are told to stop */
  while (0 != keep_receiving)  {
  
    /* See which time of receiver we are */
    if (userecvtime) {
      
      /* We are a recvtime entity */
      the_msg = recvtime(slot1, sleeptimems);
      
      /* What type of message did we get */
      if (TIMEOUT == the_msg) {
        
        /* Timed out */
        wait(print_sem);
        printf("Receiver=%d Slot=%d timed out.\n", rcvr_index, slot1);
        signal(print_sem);  
      }
      else {
        /* Got a message */
        wait(print_sem);
        printf("Receiver=%d Slot=%d received msg=%d.\n", rcvr_index, slot1, the_msg);
        signal(print_sem); 
      }
    }
    else if (0 != sleeptimems) {
      
      /* We are a recvtime entity */
      the_msg = recvclr(slot1);
      
      /* What type of message did we get */
      if (OK == the_msg) {
        /* Timed out */
        wait(print_sem);
        printf("Receiver=%d Slot=%d no message waiting.\n", rcvr_index, slot1);
        signal(print_sem);  
      }
      else {
        /* Got a message */
        wait(print_sem);
        printf("Receiver=%d Slot=%d received msg=%d.\n", rcvr_index, slot1, the_msg);
        signal(print_sem); 
      }
      
      /* See if they care about the second slot */
      if (2 <= num_slots) {
        /* We are a recvtime entity for slot 2 */
        the_msg = recvclr(slot2);
        
        /* What type of message did we get */
        if (OK == the_msg) {
          
          /* No message waiting */
          wait(print_sem);
          printf("Receiver=%d Slot=%d no message waiting.\n", rcvr_index, slot2);
          signal(print_sem);  
        }
        else {
          /* Got a message */
          wait(print_sem);
          printf("Receiver=%d Slot=%d received msg=%d.\n", rcvr_index, slot2, the_msg);
          signal(print_sem); 
        }
      }
      
      /* See if they care about the third slot */
      
      if (3 <= num_slots) {
        /* We are a recvtime entity for slot 3 */
        the_msg = recvclr(slot3);
        
        /* What type of message did we get */
        if (OK == the_msg) {
          
          /* No message waiting */
          wait(print_sem);
          printf("Receiver=%d Slot=%d no message waiting.\n", rcvr_index, slot3);
          signal(print_sem);  
        }
        else {
          /* Got a message */
          wait(print_sem);
          printf("Receiver=%d Slot=%d received msg=%d.\n", rcvr_index, slot3, the_msg);
          signal(print_sem); 
        }
      }
      
      /* Lets nod off until the next pass */
      sleepms(sleeptimems);
    }
    else {
      /* We are a standard receive entity */
      the_msg = receive(slot1);
      
      /* Got a message */
      wait(print_sem);
      printf("Receiver=%d Slot=%d received msg=%d.\n", rcvr_index, slot1, the_msg);
      signal(print_sem); 
    }
  }
   
  /* Indicate we are ending */
  wait(print_sem);
  printf("Receiver=%d is terminating.\n", rcvr_index);
  signal(print_sem);   
     
}

