#include <stdio.h>
#include <string.h>
#include <time.h>

int main() {
    // Define the source and destination buffers
    char src[8] = { 'A', 'B', 'C', 'D', 'A', 'B', 'C', 'D' };
    char dest[8];

    // Get the starting time
    clock_t start = clock();
    int iters=2400;
    size_t bytes_per_copy=2;

    for (int i = 0; i < iters; i++) {
        memcpy(dest, src, bytes_per_copy);
    }

    // Get the ending time
    clock_t end = clock();

    // Calculate the time taken in seconds
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    // Display the result
    printf("Time to copy %d bytes %d times:\n%f seconds\n", bytes_per_copy, iters,time_taken);
    printf("1 frame = %fs\n",1.0/60);
    
    getchar(); // Waits for a key press

    return 0;
}