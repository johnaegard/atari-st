#include <stdio.h>
#include <string.h>
#include <time.h>

int main() {
    // Define the source and destination buffers
    char src[8] = { 'A', 'B', 'C', 'D', 'A', 'B', 'C', 'D' };
    char dest[8];

    // Get the starting time
    clock_t start = clock();
    int iters=2500;

    // Perform the copy operation 10,000 times
    for (int i = 0; i < iters; i++) {
        memcpy(dest, src, 8);
    }

    // Get the ending time
    clock_t end = clock();

    // Calculate the time taken in seconds
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    // Display the result
    printf("Time to copy 4 words %d times:\n%f seconds\n", iters, time_taken);
    
    getchar(); // Waits for a key press

    return 0;
}