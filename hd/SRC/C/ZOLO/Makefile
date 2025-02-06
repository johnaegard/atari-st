CFLAGS = -std=c99 -O3 -I./include

zolo: lineablit.o zolo.o screen.o maze.o
	m68k-atari-mint-gcc ./build/*.o -o ZOLO.TOS
zolo.o:
	m68k-atari-mint-gcc -c ./src/zolo.c -o ./build/zolo.o ${CFLAGS}
lineablit.o:
	m68k-atari-mint-gcc -c ./src/lineablit.c -o ./build/lineablit.o ${CFLAGS}
screen.o:
	m68k-atari-mint-gcc -c ./src/screen.c -o ./build/screen.o ${CFLAGS}
maze.o:
	m68k-atari-mint-gcc -c ./src/maze.c -o ./build/maze.o ${CFLAGS}
clean:
	rm -f ./build/*.o ZOLO.TOS