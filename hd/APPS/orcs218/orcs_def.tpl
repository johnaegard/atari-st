(*
 * Template file for Modula DEFINITION Module
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
 *)

%BEGIN%
(*
 * resource set indices for %n
 *
 * created by ORCS %V
 *)

DEFINITION MODULE %R;

EXPORT QUALIFIED
%D;

CONST
%D
END %R.
