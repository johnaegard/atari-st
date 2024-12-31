README.DOC     GFA-BASIC 3.6       2.5.91
README36.TXT                                              6.5.91

The Run-Only-interpreter is no longer supported.  This version of 
the  interpreter  was  originally released  while  the  GFA-BASIC 
compiler was under development. 

GFA-BASIC 3.6 has been released to support the 68030 based  Atari 
TT.
 
The TT has two main memory areas:

ST-RAM: and TT-RAM:  


A byte in the header of the program (.PRG,  .TOS,  .TTP, or .APP) 
will determine in the program should be loaded into the ST-RAM or 
TT-RAM.  This byte (at byte offset 25) will contain the value 0,1 
or 7. If 0 or 1 the the program will load into the ST-RAM and all 
references to memory are to ST-RAM. The value of 7 will cause the 
program to load into TT-RAM.


Programs for the TT have access to a new GEMDOS function:

            adr%=GEMDOS(68,L:size,art&)

This function corresponds to MALLOC function.

The TT does not have a constant screen memory of 32000 Bytes like 
the ST. A screen could require much more memory (153600 Bytes).
The commands GET and PUT are limited to 32000 Bytes and therefore 
a  function  has  been introduced to support  the  larger  screen 
resolutions that require more than 32000 Bytes.

            bytes%=GETSIZE(x1,y1,x2,y2)

This  function  will return the number of Bytes required  by  the 
screen  between the coordinates x1,y1,x2,y2.  Several GET or  PUT 
commands could be used to address the entire screen.


The function XBIOS(4), (getrez) now returns the following values:

                                   Resolution   Colours

            0 for ST-Low res       320x200        16 
            1 for ST-Med res       640x200         4 
            2 for ST-Hi  res       640x400         2
            4 for TT-Med res       640x480        16
            6 for TT-Hi  res      1280x960         2
            7 for TT-Low res       320x480       256


The function

          TT?       returns 0      for a 68000 (normal ST)
                            1      for a 68030 or 68020
                            3      when 68881 or 68882 fitted


 The new video hardware supports the XBIOS calls:

        oshm&=XBIOS(80,shm&)        // EsetShift

Get and set TT-shift mode

        oshm&=XBIOS(81)             // Egetshift

Get TT-Shiftmode

        obnk=XBIOS(82,bnk&)         // EsetBank

Set  (or  clear)  the colour reisters.  The  TT  has  256  colour 
registers.

     ocol&=XBIOS(83,idx&,col&)   // EsetColor

Set TT colour register  #idx& to col& (&hRGB)

     ~XBIOS(84,idx&,cnt&,l:adr%) // EsetPalette

Set cn& colour from id& to the value from adr%  (eg V:a&(12))

     ~XBIOS(85,idx&,cnt&,l:adr%) //EgetPalette

Read cnt& colour

     old&=XBIOS(86,gray&)        // EsetGray

Set or clear the Gray Bit in TT-shift register.

     old&=XBIOS(87,smear&)       // EsetSmear

Set or clear the smear Bit in TT-shift register.
