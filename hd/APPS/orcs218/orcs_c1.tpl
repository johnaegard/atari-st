/*
 * Template file for C-sourcecode-output
 * (old format, Atari RCS compatible)
 * 
 * ORCS version 2.18 2020-08-20
 * 
 * %N will be replaced by the RSC filename (without path)
 * %n will be replaced by the RSC filename
 *    (in lowercase and without .rsc extension)
 * %R will be replaced by the RSC filename
 *    (according to namerules and without .rsc extension)
 * %h will be replaced by the extension of include files
 * %d marks the place where the actual data is written
 * %D will output the definitions of the object names
 * 
 * %s: Number of all strings
 * %S: Number of free strings
 * %U: Number of USERDEFs
 * %i: Number of all images
 * %B: Number of BITBLKs
 * %F: Number of free images
 * %I: Number of ICONBLKs
 * %C: Number of CICONBLKs
 * %c: Number of CICONs
 * %t: Number of TEDINFOs
 * %O: Number of OBJECTs
 * %T: Number of TREEs
 * %%: the '%'-character
 * 
 * The first line of output is after %BEGIN%
 */

%BEGIN%
/*
 * GEM resource C output of %n
 *
 * created by ORCS %V
 */

#ifndef _LONG_PTR
#  define _LONG_PTR LONG
#endif

#ifndef OS_NORMAL
#  define OS_NORMAL 0x0000
#endif
#ifndef OS_SELECTED
#  define OS_SELECTED 0x0001
#endif
#ifndef OS_CROSSED
#  define OS_CROSSED 0x0002
#endif
#ifndef OS_CHECKED
#  define OS_CHECKED 0x0004
#endif
#ifndef OS_DISABLED
#  define OS_DISABLED 0x0008
#endif
#ifndef OS_OUTLINED
#  define OS_OUTLINED 0x0010
#endif
#ifndef OS_SHADOWED
#  define OS_SHADOWED 0x0020
#endif
#ifndef OS_WHITEBAK
#  define OS_WHITEBAK 0x0040
#endif
#ifndef OS_DRAW3D
#  define OS_DRAW3D 0x0080
#endif

#ifndef OF_NONE
#  define OF_NONE 0x0000
#endif
#ifndef OF_SELECTABLE
#  define OF_SELECTABLE 0x0001
#endif
#ifndef OF_DEFAULT
#  define OF_DEFAULT 0x0002
#endif
#ifndef OF_EXIT
#  define OF_EXIT 0x0004
#endif
#ifndef OF_EDITABLE
#  define OF_EDITABLE 0x0008
#endif
#ifndef OF_RBUTTON
#  define OF_RBUTTON 0x0010
#endif
#ifndef OF_LASTOB
#  define OF_LASTOB 0x0020
#endif
#ifndef OF_TOUCHEXIT
#  define OF_TOUCHEXIT 0x0040
#endif
#ifndef OF_HIDETREE
#  define OF_HIDETREE 0x0080
#endif
#ifndef OF_INDIRECT
#  define OF_INDIRECT 0x0100
#endif
#ifndef OF_FL3DIND
#  define OF_FL3DIND 0x0200
#endif
#ifndef OF_FL3DBAK
#  define OF_FL3DBAK 0x0400
#endif
#ifndef OF_FL3DACT
#  define OF_FL3DACT 0x0600
#endif
#ifndef OF_MOVEABLE
#  define OF_MOVEABLE 0x0800
#endif
#ifndef OF_POPUP
#  define OF_POPUP 0x1000
#endif

#ifndef G_SWBUTTON
#  define G_SWBUTTON 34
#endif
#ifndef G_POPUP
#  define G_POPUP 35
#endif
#ifndef G_EDIT
#  define G_EDIT 37
#endif
#ifndef G_SHORTCUT
#  define G_SHORTCUT 38
#endif
#ifndef G_SLIST
#  define G_SLIST 39
#endif
#ifndef G_EXTBOX
#  define G_EXTBOX 40
#endif
#ifndef G_OBLINK
#  define G_OBLINK 41
#endif

#ifndef WHITEBAK
#  define WHITEBAK OS_WHITEBAK
#endif
#ifndef DRAW3D
#  define DRAW3D OS_DRAW3D
#endif
#ifndef FL3DIND
#  define FL3DIND OF_FL3DIND
#endif
#ifndef FL3DBAK
#  define FL3DBAK OF_FL3DBAK
#endif
#ifndef FL3DACT
#  define FL3DACT OF_FL3DACT
#endif

#ifndef C_UNION
#ifdef __PORTAES_H__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __GEMLIB__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __PUREC__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __ALCYON__
#  define C_UNION(x) x
#endif
#endif
#ifndef C_UNION
#  define C_UNION(x) (_LONG_PTR)(x)
#endif

%d

#define NUM_STRINGS %s
#define NUM_FRSTR %S
#define NUM_UD %U
#define NUM_IMAGES %i
#define NUM_BB %B
#define NUM_FRIMG %F
#define NUM_IB %I
#define NUM_CIB %C
#define NUM_TI %t
#define NUM_OBS %O
#define NUM_TREE %T

BYTE pname[] = "%N";
