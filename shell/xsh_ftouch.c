/* xsh_ftouch.c - xsh_ftouch */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_ftouch - shell command to create a new empty file
 *------------------------------------------------------------------------
 */
shellcmd xsh_ftouch(int nargs, char *args[])
{
	did32	fd;			/* descriptor for a file	*/

	/* For argument '--help', emit help about the 'ftouch' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s file\n\n", args[0]);
		printf("Description:\n");
		printf("\tcreates a new empty file\n");
		printf("Options:\n");
		printf("\tfile\tthe new file to create\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	if (2 != nargs)  {
    fprintf(stderr, "%s: incorrect argument\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
  }
  
  /* Lets open our file for write and it must not exist */
  fd = open(LFILESYS, args[1], "wn");
  
  if (fd == (did32)SYSERR) {
		fprintf(stderr, "%s: cannot open file %s\n",  args[0], args[1]);
		
		return 2;
	}
  
  /* File is created our job is done, tidy up by closing the file */
  close(fd);
  
	return 0;
}
