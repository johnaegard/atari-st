/* ------------------------------------------------------------------------- */
/* ----- fsel_inp.c ----- A universal fsel_(ex)input() Call -----------------*/
/* ---------------------- and routines to support Selectric ---------------- */
/* ------------------------------------------------------------------------- */
/* ----------------------------------------- (c) 1992 by Oliver Scheel ----- */
/* ------------------------------------------------------------------------- */

#include <string.h>
#include <tos.h>
#include <vdi.h>

#include "fsel_inp.h"

#define NULL   ((void *) 0l)
#define FALSE  0
#define TRUE   (!FALSE)

/* ------------------------------------------------------------------------- */

SLCT_STR *slct = NULL;
long     *fsel = NULL;

SYSHDR      *sys_header;

/* ----- Cookie Jar -------------------------------------------------------- */

typedef struct
{
   long  id,
      *ptr;
} COOKJAR;

/* ------------------------------------------------------------------------- */
/* ----- get_cookie -------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

long *get_cookie(long cookie)
{
   long  sav;
   COOKJAR  *cookiejar;
   int   i = 0;

   sav = Super((void *)1L);
   if(sav == 0L)
      sav = Super(0L);
   cookiejar = *((COOKJAR **)0x05a0l);
   sys_header = *((SYSHDR **)0x04f2L); /* ... since we are already 
                                          in super mode */
   if(sav != -1L)
      Super((void *)sav);
   if(cookiejar)
   {
      while(cookiejar[i].id)
      {
         if(cookiejar[i].id == cookie)
            return(cookiejar[i].ptr);
         i++;
      }
   }
   return(0l);
}

/* ------------------------------------------------------------------------- */
/* ----- fsel_check -------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/*
** int fsel_check(void)
**
** Function:    Checks whether a FSEL-cookie is present.
**
** Parameters:  None
**
** Return:      TRUE  FSEL-cookie present
**              FALSE -----"----- not present.
**
** ------------------------------------------------------------------------- */

int fsel_check(void)
{
   if(!fsel)
      fsel = get_cookie('FSEL');
   return(fsel ? TRUE : FALSE);
}

/* ------------------------------------------------------------------------- */
/* ----- slct_check -------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/*
** int slct_check(unsigned int version)
**
** Function:   Checks whether Selectric is installed and if it 
**             has the minimum version number required
**
** Parameters: Version contains the version number to be checked
**             (a '>=' test is done!!)
**
** Return:     TRUE  Selectric is installed and the 
**                   version number is OK.    
**             FALSE Either not installed or too low 
**                   a version number.
**
** ------------------------------------------------------------------------- */

int slct_check(unsigned int version)
{
   if(fsel_check())
   {
      slct = (SLCT_STR *)fsel;
      if(slct->id != 'SLCT')
         slct = 0L;
   }
   if(slct && (slct->version >= version))
      return(TRUE);
   else
      return(FALSE);
}

/* ------------------------------------------------------------------------- */
/* ----- file_select ------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/*
** int file_select(char *pfname, char *pname, char *fname, char *ext,
**       char *title)
**
** Function:  Calls the file selector in a simple manner.
**            During this one can pass all parts (filename, 
**            pathname etc.) individually. But one can also 
**            pass the pathname and the complete name in 'pfname'. 
**            This routine will find the missing portions for 
**            itself, within certain limits.
**            This function supportsd the FSEL-cookie and will 
**            also run without Selectric.
**
** Parameters:  *pfname  Contains finally the complete path, which
**                       can be inserted immediately into an 'open'.
**              *pname   The start path (without wildcards!).
**              *fname   A pre-selected file name.
**              *ext     An extension.
**              *title   A box title. During this a) the TOS-Version
**                       as well as b) the FSEL-Cookie is checked.
**
** Return:     The button with which the Selector was quit.
**
** Remark:     When called from Desk Accessories don't forget to 
**             place a BEG/END_UPDATE around this call!!!!!!!!!!
**             Most of the file selector clones do this 
**             (incl. Selectric), but not the original...
**
** ------------------------------------------------------------------------- */

int file_select(char *pfname, char *pname, char *fname, const char *ext, char *title)
{
   int   but;
   char  *p;

   if(!fname[0])
   {
      p = strrchr(pfname, '\\');
      if(p)
         strcpy(fname, p+1);
      else
         strcpy(fname, pfname);
   }
   if(!pname[0])
   {
      p = strrchr(pfname, '\\');
      if(p)
      {
         p[1] = '\0';
         strcpy(pname, pfname);
      }
   }
   else if(pname[strlen(pname)-1] != '\\')
      strcat(pname, "\\");
   strcat(pname, ext);

   if(fsel_check() || (sys_header->os_version >= 0x0104))
      fsel_exinput(pname, fname, &but, title);
   else
      fsel_input(pname, fname, &but);

   p = strrchr(pname, '\\');
   if(p)
      *p = '\0';
   strcpy(pfname, pname);
   strcat(pfname, "\\");
   strcat(pfname, fname);
   return(but);
}

/* ------------------------------------------------------------------------- */
/* ----- slct_extpath ------------------------------------------------------ */
/* ------------------------------------------------------------------------- */
/*
** int slct_extpath(int ext_num, char *ext[], int path_num, char *paths[])
**
** Function:   Sets user-defined extensions and paths, that are
**             then used by Selectric. The extensions and paths 
**             must be set before each Selectric call!
**
 Parameters:  ext_num     Number of the extensions
**            *ext[]      The extensions
**            path_num    Number of paths
**            *paths[]    The paths
**
** Return:    TRUE   Selectric is installed
**            FALSE  Selectric is not installed
**
** ------------------------------------------------------------------------- */

int slct_extpath(int ext_num, char *(*ext)[], int path_num, char *(*paths)[])
{
   if(slct_check(0x0100))
   {
      slct->num_ext = ext_num;
      slct->ext = ext;
      slct->num_paths = path_num;
      slct->paths = paths;
      return(TRUE);
   }
   else
      return(FALSE);
}
/* ------------------------------------------------------------------------- */
/* ----- slct_morenames ---------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/*
** int slct_morenames(int mode, int num, void *ptr)
**
** Function:   Initialises Selectric in a way that it knows 
**             that more than one name may be returned.
**
** Parameters:  mode   Gives the mode type. At present the
**                     following modes are present:
**                   0  Return files in the pointer list.
**                   1    -"-  files in a single string.
**                 num  Maximum number of names that are to 
**                      be returned.
**                *ptr  The poiter to the corresponding 
**                      structure.
**
** Return:     TRUE   Selectric is installed
**             FALSE  Selectric is not installed
**
** ------------------------------------------------------------------------- */

int slct_morenames(int mode, int num, void *ptr)
{
   if(slct_check(0x0100))
   {
      slct->comm |= CMD_FILES_OUT;
      if(mode)
         slct->comm |= CFG_ONESTRING;
      slct->out_count = num;
      slct->out_ptr = ptr;
      return(TRUE);
   }
   else
      return(FALSE);
}

/* ------------------------------------------------------------------------- */
/* ----- slct_first -------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/*
** int slct_first(DTA *mydta, int attr)
**
** Function:   If the communication-Byte has been set to 
**             CFG_FIRSTNEXT, then one can obtain the first
**             selected name with its corresponding attributes
**             via this function.
**
** Parameter:  mydta  The DTA in which the information is to 
**                    be stored.
**             attr   The attributes (see also Fsfirst). Selectric
**                    combines both attributes with AND and checks
**                    for != 0.
**
** Return:     0   OK
**            -49  No further files
**            -32  Function not present (Version < 1.02)
**
** ------------------------------------------------------------------------- */

int slct_first(DTA *mydta, int attr)
{
   if(slct_check(0x0102))
      return(slct->get_first(mydta, attr));
   else
      return(-32);
}

/* ------------------------------------------------------------------------- */
/* ----- slct_next --------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/*
** int slct_next(DTA *mydta)
**
** Function:   After the first name is returned by slct_first(),
**             one can get further names via this function.
**
** Parameters:  mydta (see above).
**
** Return:      see above.
**
** ------------------------------------------------------------------------- */

int slct_next(DTA *mydta)
{
   if(slct_check(0x0102))
      return(slct->get_next(mydta));
   else
      return(-32);
}

/* ------------------------------------------------------------------------- */
/* ----- release_dir ------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/*
** int release_dir(void)
**
** Function:    Releases the directory again (important!).
**
** Parameters:  None
**
** Return:     TRUE   Directory could be released.
**             FALSE  Error
**
** ------------------------------------------------------------------------- */

int slct_release(void)
{
   if(slct_check(0x0102))
      return(slct->release_dir());
   else
      return(-32);
}
