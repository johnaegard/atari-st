Sections:
00: "TEXT" (0-56)


Source: "AST_HelloWorld.asm"
                            	     1: 	SECTION TEXT		;CODE Section
                            	     2: 	
00:00000000 47FA0012        	     3: 	lea Message,A3
00:00000004 6142            	     4: 	jsr PrintString		;Show String Message
                            	     5:     
00:00000006 6132            	     6: 	jsr NewLine			;Move down a line
                            	     7: 
                            	     8: 	;Wait for a key before returning
00:00000008 3F3C0001        	     9: 	move.w #1,-(sp) 	;$01 - ConIn	
00:0000000C 4E41            	    10: 	trap #1				;Call GemDos
00:0000000E 548F            	    11: 	addq.l #2,sp		;Remove 1 word from stack	
                            	    12: 	
00:00000010 4267            	    13:  	clr.w -(sp)
00:00000012 4E41            	    14: 	trap #1					;Return to OS
                            	    15: 	
00:00000014 48656C6C6F20576F	    16: Message:    dc.b 'Hello World',255
00:0000001C 726C64
00:0000001F FF
                            	    17: 	even	
                            	    18: 	
                            	    19: PrintChar:
00:00000020 48E7FFFF        	    20: 	moveM.l d0-d7/a0-a7,-(sp)
00:00000024 C0BC000000FF    	    21: 		and.l #$00FF,d0	;Keep only Low Byte
00:0000002A 3F00            	    22: 		move.w d0,-(sp) ;Char (W) to show to screen
00:0000002C 3F3C0002        	    23: 		move.w #2,-(sp) ;$02 - ConOut (c_conout)
00:00000030 4E41            	    24: 		trap #1			;Call GemDos
00:00000032 588F            	    25: 		addq.l #4,sp	;Remove 2 words from stack
00:00000034 4CDFFFFF        	    26: 	moveM.l (sp)+,d0-d7/a0-a7
00:00000038 4E75            	    27: 	rts
                            	    28: 	
                            	    29: 
                            	    30: 
                            	    31: NewLine:
00:0000003A 103C000D        	    32: 	move.b #$0D,d0		;Char 13 CR
00:0000003E 61E0            	    33: 	jsr PrintChar
00:00000040 103C000A        	    34: 	move.b #$0A,d0		;Char 10 LF
00:00000044 61DA            	    35: 	jsr PrintChar
00:00000046 4E75            	    36: 	rts
                            	    37: 
                            	    38: PrintString:
00:00000048 101B            	    39: 		move.b (a3)+,d0		;Read a character in from A3
00:0000004A B03C00FF        	    40: 		cmp.b #255,d0
00:0000004E 6704            	    41: 		beq PrintString_Done;return on 255
00:00000050 61CE            	    42: 		jsr PrintChar		;Print the Character
00:00000052 60F4            	    43: 		bra PrintString
                            	    44: PrintString_Done:		
00:00000054 4E75            	    45: 	rts


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
