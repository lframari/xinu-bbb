/* xsh_fcat.c - xsh_fcat */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_fcat - shell command to dump a file to the console
 *------------------------------------------------------------------------
 */
shellcmd xsh_fcat(int nargs, char *args[])
{
	int32	nextch;			/* character read from file	*/
	did32	fd;		    	/* descriptor for a file	*/

	/* For argument '--help', emit help about the 'fcat' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s file\n\n", args[0]);
		printf("Description:\n");
		printf("\twrites contents of a given file to stdout\n");
		printf("Options:\n");
		printf("\tfile\tthe file name\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	if (2 != nargs)  {
    fprintf(stderr, "%s: incorrect argument\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
  }
  
  /* Lets open our file for read and it must exist */
  fd = open(LFILESYS, args[1], "ro");
  
  if (fd == (did32)SYSERR) {
		fprintf(stderr, "%s: cannot open file %s\n",  args[0], args[1]);
		
		return 2;
	}
  
  /* We now need to read until the end of the existing file  */
  nextch = getc(fd);
  while (nextch != EOF) {
    /* Display the current character */
    putc(stdout, nextch);
    
    /* Are there more */
    nextch = getc(fd);
	}
  
  /* Now close the file */
  close(fd);
	return 0;
}
