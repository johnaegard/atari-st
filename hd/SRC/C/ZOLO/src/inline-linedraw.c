#include <stdio.h>
#include <stdlib.h>
#include <osbind.h>

/**
 * inlined - Write a value to a memory location using inline 68000 assembly
 * @param value: The value to write
 * @param dest: Pointer to the destination memory location
 * 
 * This function demonstrates inline assembly in C for the Atari ST/TT
 * using 68000 assembly to write a value to memory.
 */
void inlined(unsigned short value, unsigned short *dest) {
    /* 
     * move.w d0,a0@  - Move the word in d0 to the address in a0
     * 
     * The compiler will load:
     * - value into d0
     * - dest address into a0
     */
    asm volatile (
        "move.w d0,a0@"    /* Write d0 to the address in a0 */
        : /* no outputs */
        : /* inputs already in d0 and a0 */
    );
}

/**
 * Main function to demonstrate the inlined assembly function
 */
int main() {
    unsigned short test_value = 0x1234;
    unsigned short memory_location = 0;
    
    printf("Before: memory_location = 0x%04X\n", memory_location);
    
    /* Call our inline assembly function */
    inlined(test_value, &memory_location);
    
    printf("After: memory_location = 0x%04X\n", memory_location);
    
    /* Verify the value was written correctly */
    if (memory_location == test_value) {
        printf("Success! The value was written correctly.\n");
    } else {
        printf("Error: The value was not written correctly.\n");
    }
    
    /* Wait for a keypress before exiting */
    printf("\nPress any key to exit...");
    Cconin();
    
    return 0;
}
