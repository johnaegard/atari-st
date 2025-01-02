#include <stdio.h>
#include <string.h>
#include <time.h>

int main() {
    // Define the source and destination buffers
    char src[4] = { 'A', 'B', 'C', 'D' };
    char dest[4];

    // Get the starting time
    clock_t start = clock();

    // Perform the copy operation 10,000 times
    for (int i = 0; i < 10000; i++) {
        memcpy(dest, src, 4);
    }

    // Get the ending time
    clock_t end = clock();

    // Calculate the time taken in seconds
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    // Display the result
    printf("Time taken to copy a 4-byte block 10,000 times: %f seconds\n", time_taken);
    
    getchar(); // Waits for a key press

    return 0;
}