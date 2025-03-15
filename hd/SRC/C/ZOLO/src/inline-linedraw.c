#include <stdio.h>
#include <stdlib.h>
#include <osbind.h>  /* For Physbase() */

/**
 * inlined_line_draw - Fill memory with a value at regular intervals
 * @param start_addr: Starting memory address
 * @param end_addr: Ending memory address (inclusive)
 * @param increment: Number of bytes to increment between each write
 * @param value: Value to write at each position
 *
 * This function uses inline 68000 assembly to efficiently fill memory
 * at regular intervals. It writes the value at start_addr, then at
 * start_addr+increment, and so on until it reaches or exceeds end_addr.
 */
void inlined_line_draw(void *start_addr, void *end_addr, long increment, unsigned short value) {
    if (start_addr > end_addr) return;
    __asm__ __volatile__ (
        "move.l %0,a0\n\t"    /* Load start address into a0 */
        "move.l %1,a1\n\t"    /* Load end address into a1 */
        "move.w %2,d0\n\t"    /* Load value into d0 */
        "move.l %3,d1\n\t"    /* Load increment into d1 (as long) */
        
        "1:\n\t"              /* Local label for loop start */
        "move.w d0,(a0)\n\t"  /* Store d0 at current address */
        "add.l  d1,a0\n\t"    /* Increment address by d1 (long operation) */
        "cmp.l  a1,a0\n\t"    /* Compare current address with end address */
        "ble.s  1b\n\t"       /* Loop if current <= end address */
        : /* no outputs */
        : "g" (start_addr), "g" (end_addr), "g" (value), "g" (increment)
        : "a0", "a1", "d0", "d1", "memory" /* clobbered registers ajnd memory */
    );
}

/**
 * Main function to demonstrate the usage of inlined_line_draw
 */
int main() {
    /* Get the physical screen base address */
    void *physbase = Physbase();
    
    /* Starting address (Physbase + 80 bytes) */
    unsigned short *start = (unsigned short *)((char *)physbase + 80);
    
    /* End address (enough space for several iterations) */
    unsigned short *end = (unsigned short *)((char *)physbase + 16000);
    
    /* Value to write */
    unsigned short value = 0xFFFF;
    
    printf("Drawing vertical line pattern on screen...\n");
    printf("Press Enter to start drawing\n");
    getchar();
    
    /* Call our assembly function to draw a pattern */
    inlined_line_draw((void*)start, (void*)end, 160, value);
    
    printf("Drawing complete. Press Enter to exit...\n");
    getchar();
    
    return 0;
}
