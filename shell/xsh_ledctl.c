/* xsh_ledctl.c - xsh_ledctl */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/* LED GPIO SIGNAL PROC PIN 
USR0 GPIO1_21 V15
USR1 GPIO2_22 U15
USR2 GPIO2_23 T15
USR3 GPIO2_24 V16 */

/* LEDs GPIO BASE */
#define GPIO_BASE_USR0_LED  GPIO1_BASE
#define GPIO_BASE_USR1_LED  GPIO1_BASE  
#define GPIO_BASE_USR2_LED  GPIO1_BASE  
#define GPIO_BASE_USR3_LED  GPIO1_BASE      
      
/* LEDs GPIO PIN */
#define GPIO_PIN_USR0_LED  GPIO_PIN_21
#define GPIO_PIN_USR1_LED  GPIO_PIN_22 
#define GPIO_PIN_USR2_LED  GPIO_PIN_23  
#define GPIO_PIN_USR3_LED  GPIO_PIN_24

/* LED control functions */

/* This function turns on the LED using the GPIO Base Register 
   and PIN number */
void gpio_ctrl_led_on(struct gpio_csreg* gpioBase, uint32 pinNumber)
{
  /* Index we want to control the given pin */
  gpioBase->oe &= ~pinNumber;
  
  /* Set the bit on to turn on the LED */
  gpioBase->set_data = pinNumber;
}

/* This function turns off the LED using the GPIO Base Register 
   and PIN number */
void gpio_ctrl_led_off(struct gpio_csreg* gpioBase, uint32 pinNumber)
{
  /* Index we want to control the given pin */
  gpioBase->oe &= ~pinNumber;
  
  /* Clear the bit on to turn off the LED */
  gpioBase->clear_data = pinNumber;
}

/*------------------------------------------------------------------------
 * xsh_ledctl  -  Shell command to control a LED on the BeagleBone Black
 *------------------------------------------------------------------------
 */
shellcmd xsh_ledctl(int nargs, char *args[])
{
  int32 ledcmd; /* 0 = turn off, 1 = turn on */
	char	*chptr;	/* Walks through argument	*/
	char	ch;			/* Next character of argument	*/
  
  /* The base register for the GPIO command */
  struct gpio_csreg* gpioBase = 0;
  
  /* The pin number */
  uint32 gpioPin = 0;

	/* For argument '--help', emit help about the 'ledctl' command	*/
	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tControls the specified LED on the BeagleBone Black\n");
    printf("First argument is the LED (0, 1, 2 or 3) (0 = usr0, 1=usr1 etc)\n");
    printf("Second argument is --on or --off for on or off");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid number of arguments */
	if (nargs > 3) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	if (nargs != 3) {
		fprintf(stderr, "%s: argument in error\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

  /* Get the LED and configure its GPIO Base and PIN */
	chptr = args[1];
	ch = *chptr++;
  if (*chptr != '\0')
  {
    fprintf(stderr, "%s: LED argument must be 0, 1, 2, 3\n",
				args[0]);
	  return 1;
  }
  else if (ch == '0')
  {
    gpioBase = GPIO_BASE_USR0_LED;
    gpioPin  = GPIO_PIN_USR0_LED;
  }
  else if (ch == '1')
  {
    gpioBase = GPIO_BASE_USR1_LED;
    gpioPin  = GPIO_PIN_USR1_LED;
  }
  else if (ch == '2')
  {
    gpioBase = GPIO_BASE_USR2_LED;
    gpioPin  = GPIO_PIN_USR2_LED;
  }
  else if (ch == '3')
  {
    gpioBase = GPIO_BASE_USR3_LED;
    gpioPin  = GPIO_PIN_USR3_LED;
  }
  else
  {
    fprintf(stderr, "%s: LED argument must be 1, 2, 3\n",
				args[0]);
	  return 1;
  }
  
  /* determine the command */
  if (strncmp(args[2], "--on", strlen("--on")) == 0) 
  {
    /* turn it on */
    ledcmd = 1;
  }
  else if (strncmp(args[2], "--off", strlen("--off")) == 0) 
  {
    /* turn it off */
    ledcmd = 0;
  }
  else
  {
    /* oops bad command */
    fprintf(stderr, "%s: LED command must be --on or --off\n",
				args[0]);
	  return 1;
  }
	
  if (1 == ledcmd)
  {
    gpio_ctrl_led_on(gpioBase, gpioPin);
  }
  else
  {
    gpio_ctrl_led_off(gpioBase, gpioPin);
  }
 
	return 0;
}
