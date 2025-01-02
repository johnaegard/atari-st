#include <stddef.h>
#include <tos.h>
#include "..\distrib\source\jclkcook.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

static JCLKSTRUCT *clocky = NULL;
static char *hotkey = NULL;
static int ehc_id = 0;

int	cookie (long cookie, long *value);

/*
** Initialise hotkeys
** 
** Returns: positive number between 1 and 127 - this is the application's EHC ID
**       or negative number when an error occurs
**          -1 = Clocky not found
**          -2 = Clocky found but with incompatible data structure
**          -3 = too many clients are already installed
*/
int initHotkeys(void)
{
	int used, i;

	/* Required version of Clocky installed? */
	if (cookie(CLOCKY_IDENT, (void *)&clocky))
	{
		if (clocky->name != CLOCKY_IDENT_NUM || (clocky->version / 0x100) != (JCLKSTRUCT_VERSION / 0x100))
			return -2;	/* Needs a different version */
	}
	else
		return -1;		/* Not installed at all */
	
	hotkey = (char *)(&clocky->actual_key);

	/* Get a EHC ID */
	ehc_id = 0;
	do {
		ehc_id++;
		used = FALSE;
		for(i = 0; i < 128; i++)
		{
			if (clocky->ehc_table[i] == ehc_id)
			{
				used = TRUE;	/* this id is already in use */
				break;			/* try higher mark */
			}
		}
	} while(used && (ehc_id < 128));

	if (ehc_id > 127) /* Too many hotkey-clients */
		return -3;

	/* OK, return the EHC ID */
	return ehc_id;
}


/*
** Register a hotkey
*/

int registerHotkey(char key, char flags)
{
	/* Successfully initialised? */
	if (!ehc_id)
		return FALSE;

	/* Free key? */
	if (clocky->ehc_table[key] > 0)
		return FALSE;
		
	/* Register key */
	clocky->ehc_table[key] = ehc_id;
	
	if (clocky->version >= 0x300)
		clocky->ehc_table[key + 128] = flags;

	return ehc_id;
}


/*
** Remove registered hotkeys
**
** key == -1 -> Remove all registered hotkeys
** otherwise remove 'key'.
*/

void removeHotkeys(int key)
{
	int teller;

	if (ehc_id != 0)
	{
		if (key == -1) /* Remove all registered keys */
		{
			for (teller = 0; teller < 128; teller++)
				if (clocky->ehc_table[teller] == ehc_id)
					clocky->ehc_table[teller] = 0;
		}
		else	/* Remove 'key' */
		{
			if (clocky->ehc_table[key] == ehc_id)
				clocky->ehc_table[key] = 0;
		}
	}
}


/*
** Check hotkeys
**
** Returns the value of the pressed hotkey in lower byte
**         and the current state of shift keys in upper byte
** If no hotkey is pressed, or it's not one of yours, this
** function returns 0.
*/

int checkHotkeys(void)
{
	/* Return ASAP if no hotkey is pressed */
	if (*hotkey == 0)
		return 0;

	/* Hotkey pressed, but is it mine? */
	if (clocky->ehc_table[*hotkey] == ehc_id)
	{
		int retValue = *hotkey;
		*hotkey = 0;

		if (retValue > 0 && clocky->version >= 0x300)
			retValue |= (clocky->actual_shift >> 8);
	
		return retValue; /* Return hotkey and shift keys */
	}

	/* Nope */
	return 0;
}


/*
** Scancode -> ASCII
*/

char scan2ascii(char scan)
{
	struct keytab {
		unsigned char *unshift;
		unsigned char *shift;
		unsigned char *capslock;
		} *ktab;

	ktab = (struct keytab *)Keytbl((char *)-1,(char *)-1,(char *)-1);
	return ktab->unshift[scan];
}


/*
** ASCII -> Scancode
*/

char ascii2scan(char ascii)
{
	int teller;

	/* Test all scancodes until a match is found */
	for (teller = 0; teller < 128; teller++)
		if (scan2ascii(teller) == ascii)
			return teller;

	/* Hmmm... It doesn't have a key */			
	return 0;
}


/*
** Get the value of a cookie
** Stolen from E-Gem 2.10
*/

int	cookie (long cookie, long *value)
{
	register long *cookiejar, old_stack;
	
	old_stack = Super (NULL);
	cookiejar = *((long **) 0x5a0l);
	Super ((void *) old_stack);
	
	if (cookiejar)
	{
		while (*cookiejar)
		{
			if (*cookiejar==cookie)
			{
				if (value)
					*value = *++cookiejar;
				return(TRUE);
			}
			cookiejar += 2;
		}
	}
	return(FALSE);
}
