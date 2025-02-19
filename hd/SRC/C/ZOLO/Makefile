CFLAGS = -std=c99 -O3 -g -I./include

sym: zolo
	gst2ascii ZOLO.TOS | m68k-atari-mint-c++filt > zolo.sym
bench: bench.o sprite.o lineablit.o image.o
	m68k-atari-mint-gcc ./build/lib/*.o ./build/bench.o -o BENCH.TOS
bench.o:
	m68k-atari-mint-gcc -c ./src/bench.c -o ./build/bench.o ${CFLAGS}
zolo: lineablit.o zolo.o image.o maze.o sprite.o
	m68k-atari-mint-gcc ./build/lib/*.o ./build/zolo.o -o ZOLO.TOS
zolo.o:
	m68k-atari-mint-gcc -c ./src/zolo.c -o ./build/zolo.o ${CFLAGS}
lineablit.o:
	m68k-atari-mint-gcc -c ./src/lineablit.c -o ./build/lib/lineablit.o ${CFLAGS}
image.o:
	m68k-atari-mint-gcc -c ./src/image.c -o ./build/lib/image.o ${CFLAGS}
maze.o:
	m68k-atari-mint-gcc -c ./src/maze.c -o ./build/lib/maze.o ${CFLAGS}
sprite.o:
	m68k-atari-mint-gcc -c ./src/sprite.c -o ./build/lib/sprite.o ${CFLAGS}
inline:
	m68k-atari-mint-gcc ./src/inline.c -o INLINE.TOS  ${CFLAGS}
clean:
	rm -f ./build/*.o ./build/lib/*.o *.TOS