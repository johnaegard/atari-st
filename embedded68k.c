#include <stdio.h>

/**
 * Copies the value in D0 to the memory location pointed to by A0
 * 
 * @param value The value to store in memory
 * @param dest Pointer to the destination memory location
 */
void inlined(long value, long *dest) {
    /* 
     * move.l d0,(a0) - Copy the long value in D0 to the memory location pointed to by A0
     * 
     * In GCC inline assembly:
     * - Input operands: value -> d0, dest -> a0
     * - Output operands: none
     * - Clobbered registers: none (the operation doesn't modify any registers)
     */
    asm volatile (
        "move.l %0,(%1)"    /* Copy the value in D0 to the memory location in A0 */
        : /* no outputs */
        : "d" (value), "a" (dest) /* inputs: value in d0, dest in a0 */
        : /* no clobbers */
    );
}

int main() {
    long value = 0x12345678;  /* Test value (hexadecimal) */
    long result = 0;          /* Variable to store the result */
    
    printf("Before: value = 0x%08lx, result = 0x%08lx\n", value, result);
    
    /* Call our assembly function to copy value to result */
    inlined(value, &result);
    
    printf("After:  value = 0x%08lx, result = 0x%08lx\n", value, result);
    
    /* Verify the copy worked correctly */
    if (result == value) {
        printf("Success! The value was copied correctly.\n");
    } else {
        printf("Error! The value was not copied correctly.\n");
    }
    
    return 0;
}
