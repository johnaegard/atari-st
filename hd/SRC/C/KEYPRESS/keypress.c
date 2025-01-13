#include <osbind.h> // Provides BIOS and XBIOS function prototypes
#include <stdio.h>  // For printf

int main() {
    long key;
    printf("Press any key to test (press 'q' to quit):\n");
    while (1) {
        key = Bconstat(2); // Check if a key is available
        if (key) { // Non-zero if a key is pressed
            key = Bconin(2); // Read the key
            char ch = key & 0xFF; // Extract ASCII character
            char c = (key >> 16) & 0xFF;  // extract scancode
            printf("%c -- %d\n", ch, (key >> 16) & 0XFF);
            if (ch == 'q') break;
        }
    }
    return 0;
}