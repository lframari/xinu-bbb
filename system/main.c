/*  main.c  - main */

#include <xinu.h>

process	main(void)
{

	/* Run the Xinu shell */

	recvclr(MSG_ALL_SLOTS);
  printf("Hello from Louis Framarini!\n\n");
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive(0); /* Wait for the process killed message */
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
    
}
