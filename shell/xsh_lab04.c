/* xsh_lab04.c - xsh_lab04 */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/* Global variables for lab 04 */

/* Variable to indicate if we should use the starvation path 
   or the valid solution path */
uint32 starve_path;
 
/* Variable to indicate we should still go on */
uint32 keep_looping;

/* Number of philosophers */
#define NPHIL 5

/* Utensils represented as a semaphore */
sid32 utensils[NPHIL];

/* The process IDs */
pid32 pids[NPHIL];

/* semaphore to ensure print finishes */
sid32 print_sem;

/* The left utensil */
#define leftutensil(i) (utensils[i])

/* The right  utensil */
#define rightutensil(i) (utensils[(i+1)%NPHIL])


/*
* Forward Declare the philosopher function
*/
void philosopher(int32 phil_id);


/*------------------------------------------------------------------------
 * xsh_lab02  -  Shell command to execute lab 04
 *------------------------------------------------------------------------
 */
shellcmd xsh_lab04(int nargs, char *args[])
{
  
  /* Default our path */
  starve_path = 0;
  
  /* Have everyone keep looping */
  keep_looping = 1;
  
	/* For argument '--help', emit help about the 'lab04' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tRun Lab 04 from CIS 657\n");
    printf("\tIt illustrates the dining philosopher problem.\n");
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
  
  if (nargs == 2 && strncmp(args[1], "--starve", 9) == 0) {
    /* Try the starve */
    starve_path = 1;
  }
  
  /* Loop counter */
  int32 i;
  
  /* The name of the philosopher */
  char philname[PNMLEN];

  /* Create the utensils for everyone */
  for (i = 0; i < NPHIL; i++) {
    utensils[i] = semcreate(1);
  }
  
  /* create the print semaphore */
  print_sem = semcreate(1);
  
  printf("Hit carriage return when done\n\n");
  
  /* Create the philosophers and let them run wild */
  for (i = 0; i < NPHIL; i++) {
    
    /* Give the philosopher a name */
    sprintf(philname, "Philosopher %d", i);
    
    /* Create the philosopher and let he/she go */
    pids[i] = create(philosopher, 1024, 20, philname, 1, i);
    resume(pids[i]);
  }
  
  /* Wait for our CR */
  getc(CONSOLE);
  
  /* Lets clean up */
  for (i = 0; i < NPHIL; i++) {
    kill(pids[i]);
  }
  
	return 0;
}

/* Obtains the utensils */
void grab_utensils(int32 phil_id)
{
  if (0 != starve_path) {
    wait(leftutensil(phil_id));
    wait(print_sem);
    printf ("%d left up\n", phil_id);
    signal(print_sem);
     
    /* Required for our deadlock */
    sleepms(1); 
    
    wait(rightutensil(phil_id)); 
    wait(print_sem);
    printf ("%d right up\n", phil_id);
    signal(print_sem);
  }
  else {
  
    /* Alternate the odd and even grabbing the other way */
    if (0 == (phil_id % 2)){
      
      wait(leftutensil(phil_id)); 
      wait(print_sem);
      printf ("%d left up\n", phil_id);
      signal(print_sem);
      
      /* Remains in to show a similar paradigm */
      sleepms(1);  
      
      wait(rightutensil(phil_id));
      wait(print_sem);
      printf ("%d right up\n", phil_id);
      signal(print_sem);
    }
    else {
      
      wait(rightutensil(phil_id)); 
      wait(print_sem);
      printf ("%d left up\n", phil_id);
      signal(print_sem);
      
      /* Again uniformity */
      sleepms(1);  
      
      wait(leftutensil(phil_id));
      wait(print_sem);
      printf ("%d left up\n", phil_id);
      signal(print_sem);   
     }
  }
}

/* Gives back the utensils */
void put_down_utensils(int32 phil_id)
{
  if (0 != starve_path) {
    
    signal(rightutensil(phil_id));
    wait(print_sem);
    printf ("%d right down\n", phil_id);
    signal(print_sem);
    
    signal(leftutensil(phil_id));
    wait(print_sem);
    printf ("%d left down\n", phil_id);
    signal(print_sem);    
  }
  else {
    /* Alternate the odd and even giving back in reverse */
    if (0 == (phil_id % 2)){
      
      signal(rightutensil(phil_id));
      wait(print_sem);
      printf ("%d right down\n", phil_id);
      signal(print_sem);
      
      signal(leftutensil(phil_id));    
      wait(print_sem);
      printf ("%d left down\n", phil_id);
      signal(print_sem);
    }
    else {
      
      signal(leftutensil(phil_id));   
      wait(print_sem);
      printf ("%d left down\n", phil_id);
      signal(print_sem);
      
      signal(rightutensil(phil_id));
      wait(print_sem);
      printf ("%d right down\n", phil_id);
      signal(print_sem);
    }
  }
}

/* simple eat routine */
void eat(int32 phil_id)
{
  /* Indicate we are eating */
  wait(print_sem);
  printf ("%d eat\n", phil_id);
  signal(print_sem);
  
  /* Let things digest */
  sleepms(1);
}

/* Simple think function */
void think(int32 phil_id)
{  
 /* Indicate we are done eating */
  wait(print_sem);
  printf ("%d think\n", phil_id);
  signal(print_sem);
  
  /* Let the knowledge sink in */
  sleepms(phil_id);
}

/*
* philosopher: implement a single philosopher
*/
void philosopher(int32 phil_id)
{
 
  while (0 != keep_looping) {
    
    /* Hungry dine first get our utensils */
    grab_utensils(phil_id);
    
    /* Got the utensils time to eat */
    eat(phil_id);
    
    /* Done eating do the right thing and share our dirty utensils */
    put_down_utensils(phil_id);
    
    /* ponder the ineffable mysteries of life */
    think(phil_id);
  }
}
