CFLAGS = -std=c99 -O3 -g -I./include

sym: zolo
	gst2ascii ZOLO.TOS | m68k-atari-mint-c++filt > zolo.sym
zolo: lineablit.o zolo.o image.o maze.o sprite.o
	m68k-atari-mint-gcc ./build/*.o -o ZOLO.TOS
zolo.o:
	m68k-atari-mint-gcc -c ./src/zolo.c -o ./build/zolo.o ${CFLAGS}
lineablit.o:
	m68k-atari-mint-gcc -c ./src/lineablit.c -o ./build/lineablit.o ${CFLAGS}
image.o:
	m68k-atari-mint-gcc -c ./src/image.c -o ./build/image.o ${CFLAGS}
maze.o:
	m68k-atari-mint-gcc -c ./src/maze.c -o ./build/maze.o ${CFLAGS}
sprite.o:
	m68k-atari-mint-gcc -c ./src/sprite.c -o ./build/sprite.o ${CFLAGS}
clean:
	rm -f ./build/lib/*.o ./build/*.o *.TOS
profile: sym
	hatari_profile.py -st -r zolo.sym zolo.profile | tail -n 6