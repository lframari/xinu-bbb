/* xsh_fappend.c - xsh_fappend */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_fappend - shell command to read from the terminal until EOF and 
 *               append that data to the the named file
 *------------------------------------------------------------------------
 */
shellcmd xsh_fappend(int nargs, char *args[])
{
	int32	nextch;			/* character read from file	*/
	did32	fd;			/* descriptor for a file	*/


	/* For argument '--help', emit help about the 'xsh_fappend' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s file\n\n", args[0]);
		printf("Description:\n");
		printf("\twrites contents of files or stdin to stdout\n");
		printf("Options:\n");
		printf("\tfile\tfile name to which the user entered data will be appended\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}
  
  if (2 != nargs)  {
    fprintf(stderr, "%s: incorrect argument\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
  }
  
  /* Lets open our file read and write */
  fd = open(LFILESYS, args[1], "rw");
  
  if (fd == (did32)SYSERR) {
		fprintf(stderr, "%s: cannot open file %s\n",  args[0], args[1]);
		
		return 2;
	}
  
  /* We now need to read until the end of the existing file*/
  nextch = getc(fd);
  while (nextch != EOF) {
    nextch = getc(fd);
	}
  
  /* We are at the end of the file, copy the user input out until the end */
  nextch = getc(stdin);
	while (nextch != EOF) {
		putc(fd, nextch);
		nextch = getc(stdin);
	}
  
  /* Close the file */
  close(fd);

	return 0;
}
