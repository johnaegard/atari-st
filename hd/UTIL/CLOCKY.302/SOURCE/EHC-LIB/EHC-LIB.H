/*
** [E]xternal [H]otkey [C]lient
** ----------------------------
** Library for using [Control]+[Alt] hotkeys with Clocky
**
** (c) Petr Stehlik
**     Jo Even Skarstein 1998
*/


/*
** Initialise hotkeys
** ------------------
** Call this to initialise this library
** 
** Returns: positive number between 1 and 127 - this is the application's EHC ID
**       or negative number when an error occurs
**          -1 = Clocky not found
**          -2 = Clocky found but with incompatible data structure
**          -3 = too many clients are already installed
*/
int initHotkeys(void);

/*
** Register a hotkey
** -----------------
** 'key' is the scancode for the hotkey you wish to register.
** 'flags' is a bit field: bit 0 = hotkey passthrough
**                         bit 1 = allow shifts
*/
#define EHC_PASSTHROUGH	0x01
#define EHC_ALLOWSHIFTS	0x02
int registerHotkey(char key, char flags);

/*
** Remove registered hotkeys
** -------------------------
** (key == -1) -> Remove all registered hotkeys
** otherwise remove 'key'.
*/
void removeHotkeys(int key);

/*
** Check hotkeys
** -------------
** Returns the value of the pressed hotkey in lower byte
**         and the current state of shift keys in upper byte
** If no hotkey is pressed, or it's not one of yours, this
** function returns 0.
*/
int checkHotkeys(void);

/*
** Scancode -> ASCII
** -----------------
*/
char scan2ascii(char scan);

/*
** ASCII -> Scancode
** -----------------
*/
char ascii2scan(char ascii);
