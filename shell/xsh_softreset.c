/* xsh_softreset.c - xsh_softreset */

#include <xinu.h>
#include <stdio.h>
#include <string.h>


/*------------------------------------------------------------------------
 * xsh_softreset  -  Shell command to soft reset the BeagleBone Black
 *------------------------------------------------------------------------
 */
shellcmd xsh_softreset(int nargs, char *args[])
{
  struct	watchdog_csreg *wdtptr;	/* Watchdog registers		*/
  
	/* For argument '--help', emit help about the 'softreset' command	*/
	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tPerforms a somewhat soft reset on the BeagleBone Black\n");
    printf("\tusing the watchdog timer. The timeout is about 5 seconds.\n");
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
  
  /* get the watchdog */
  wdtptr = (struct watchdog_csreg *)WDTADDR;
  
  /* initialize prescaler */
  while(wdtptr->wwps & 0x00000001);

  /* a little more setup */
  wdtptr->wclr = 0x00000020;        
  while(wdtptr->wwps & 0x00000001);
    
  /* Attempt to set a large value for a quick reset using the loader */
  while(wdtptr->wwps & 0x00000004);
  wdtptr->wldr = 0xfffe0000;
  while(wdtptr->wwps & 0x00000004);
  
  /* Attempt to set a large value for a quick reset using the counter */
  while(wdtptr->wwps & 0x00000002);
  wdtptr->wcrr = 0xfffe0000;
  while(wdtptr->wwps & 0x00000002);
  
	/* Enable the watchdog timer */
	wdtptr->wspr = 0x0000bbbb;
	while(wdtptr->wwps & 0x00000010);
	wdtptr->wspr = 0x00004444;
	while(wdtptr->wwps & 0x00000010);
  
  printf("Goodbye\n");
  
  /* Sleep so we do not start anything else */
  sleep(20);

  /* We willl never get here */
	return 0;
}
