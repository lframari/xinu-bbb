/* xsh_ftruncate.c - xsh_ftruncate */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_ftruncate - shell command to truncate a file
 *------------------------------------------------------------------------
 */
shellcmd xsh_ftruncate(int nargs, char *args[])
{
	did32	fd;			/* descriptor for a file	*/
	syscall result;   /* Result of truncate */ 


	/* For argument '--help', emit help about the 'cat' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s [file]\n\n", args[0]);
		printf("Description:\n");
		printf("\truncates the contents of a given file\n");
		printf("Options:\n");
		printf("\tfile...\tzero or more file names\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}
  
  if (2 != nargs)  {
    fprintf(stderr, "%s: incorrect argument\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
  }
  
  /* Lets open our file write and it must exist */
  fd = open(LFILESYS, args[1], "wo");
  
  if (fd == (did32)SYSERR) {
		fprintf(stderr, "%s: cannot open file %s\n",  args[0], args[1]);
		
		return 2;
	}
  
  /* Now through the control function we can truncate */
  result = control(fd, LF_CTL_TRUNC, 0, 0);
  
  if (result == SYSERR) {
		fprintf(stderr, "%s: cannot truncate file %s\n",  args[0], args[1]);
		close(fd);
		return 2;
	}
  
  /* Now close the file */
  close(fd);

	return 0;
}
