Sections:
00: "TEXT" (0-56)


Source: "src/AST_HelloWorld.asm"
                            	     1: 
                            	     2: 	SECTION TEXT		;CODE Section
                            	     3: 	
00:00000000 47FA0012        	     4: 	lea Message,a3
00:00000004 6142            	     5: 	jsr PrintString		;Show String Message
                            	     6:     
00:00000006 6132            	     7: 	jsr NewLine			;Move down a line
                            	     8: 
                            	     9: 	;Wait for a key before returning
00:00000008 3F3C0001        	    10: 	move.w #1,-(sp) 	;$01 - ConIn	
00:0000000C 4E41            	    11: 	trap #1				;Call GemDos
00:0000000E 548F            	    12: 	addq.l #2,sp		;Remove 1 word from stack	
                            	    13: 	
00:00000010 4267            	    14:  	clr.w -(sp)
00:00000012 4E41            	    15: 	trap #1					;Return to OS
                            	    16: 	
00:00000014 48656C6C6F20576F	    17: Message:    dc.b 'Hello World',255
00:0000001C 726C64
00:0000001F FF
                            	    18: 	even	
                            	    19: 	
                            	    20: PrintChar:
00:00000020 48E7FFFF        	    21: 	moveM.l d0-d7/a0-a7,-(sp)
00:00000024 C0BC000000FF    	    22: 		and.l #$00FF,d0	;Keep only Low Byte
00:0000002A 3F00            	    23: 		move.w d0,-(sp) ;Char (W) to show to screen
00:0000002C 3F3C0002        	    24: 		move.w #2,-(sp) ;$02 - ConOut (c_conout)
00:00000030 4E41            	    25: 		trap #1			;Call GemDos
00:00000032 588F            	    26: 		addq.l #4,sp	;Remove 2 words from stack
00:00000034 4CDFFFFF        	    27: 	moveM.l (sp)+,d0-d7/a0-a7
00:00000038 4E75            	    28: 	rts
                            	    29: 	
                            	    30: 
                            	    31: 
                            	    32: NewLine:
00:0000003A 103C000D        	    33: 	move.b #$0D,d0		;Char 13 CR
00:0000003E 61E0            	    34: 	jsr PrintChar
00:00000040 103C000A        	    35: 	move.b #$0A,d0		;Char 10 LF
00:00000044 61DA            	    36: 	jsr PrintChar
00:00000046 4E75            	    37: 	rts
                            	    38: 
                            	    39: PrintString:
00:00000048 101B            	    40: 		move.b (a3)+,d0		;Read a character in from A3
00:0000004A B03C00FF        	    41: 		cmp.b #255,d0
00:0000004E 6704            	    42: 		beq PrintString_Done;return on 255
00:00000050 61CE            	    43: 		jsr PrintChar		;Print the Character
00:00000052 60F4            	    44: 		bra PrintString
                            	    45: PrintString_Done:		
00:00000054 4E75            	    46: 	rts
                            	    47: 	
                            	    48: 


Symbols by name:
Message                         00:00000014
NewLine                         00:0000003A
PrintChar                       00:00000020
PrintString                     00:00000048
PrintString_Done                00:00000054

Symbols by value:
00000014 Message
00000020 PrintChar
0000003A NewLine
00000048 PrintString
00000054 PrintString_Done
