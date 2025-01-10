/*
 * resource set indices for emucurs
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        0
 * Number of Bitblks:        8
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       0
 * Number of Free Strings:   0
 * Number of Free Images:    8
 * Number of Objects:        0
 * Number of Trees:          0
 * Number of Userblks:       0
 * Number of Images:         8
 * Total file size:          772
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "emucurs"
#endif
#undef RSC_ID
#ifdef emucurs
#define RSC_ID emucurs
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 0
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 8
#define NUM_BB 8
#define NUM_FRIMG 8
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 0
#define NUM_OBS 0
#define NUM_TREE 0
#endif



#define ARROW      0 /* Mouse cursor */

#define TEXT       1 /* Mouse cursor */

#define HRGLASS    2 /* Mouse cursor */

#define POINT      3 /* Mouse cursor */

#define FLAT       4 /* Mouse cursor */

#define THIN_CR    5 /* Mouse cursor */

#define THICK_CR   6 /* Mouse cursor */

#define OUTLN_CR   7 /* Mouse cursor */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD emucurs_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD emucurs_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD emucurs_rsc_free(void);
#endif
