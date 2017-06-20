/* xsh_fls.c - xsh_fls */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_fls - shell command to list the Xinu File directory
 *------------------------------------------------------------------------
 */
shellcmd xsh_fls(int nargs, char *args[])
{
	uint32 i;			                                  /* loop index		*/
	uint32 numentries;                              /* number of entries */
  struct lfsdirentry direntries[LF_NUM_DIR_ENT];  /* Directory entries */


	/* For argument '--help', emit help about the 'fls' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tlists the files in the main directory\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}
  
  if (1 != nargs) {
    fprintf(stderr, "%s: incorrect argument\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
  }
  
  /* get the data */
  lfsinfo(&numentries, direntries);
  
  /* Now loop through them */
 
  printf("\nFilename\t\tSize\n");
  printf("--------\t\t----\n");
  for (i = 0; i < numentries; ++i) {
    printf("%-16s\t%-6d\n", direntries[i].filename, direntries[i].filesize);
  }
  
  printf("\nNumber of files=%d\n\n", numentries);

	return 0;
}
