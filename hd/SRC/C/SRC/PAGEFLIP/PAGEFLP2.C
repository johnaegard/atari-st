#include <osbind.h>  // For GEMDOS system calls
#include <stdlib.h>
#include <stdio.h>

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200
#define SCREEN_SIZE   (SCREEN_WIDTH * SCREEN_HEIGHT / 8)  // In bytes (assuming 1 bit per pixel)
#define NUM_BUFFERS   2

// Function to clear a screen buffer
void clear_screen(void *buffer) {
    for (int i = 0; i < SCREEN_SIZE; i++) {
        ((char *)buffer)[i] = 0;  // Fill the buffer with zeros
    }
}

// Function to draw a simple animation frame
void draw_frame(void *buffer, int frame) {
    unsigned short *screen = (unsigned short *)buffer;
    int offset = frame % SCREEN_WIDTH;  // Simple animation offset

    for (int y = 50; y < 150; y++) {
        for (int x = 50 + offset; x < 70 + offset; x++) {
            screen[(y * SCREEN_WIDTH + x) / 16] |= (1 << (15 - (x % 16)));
        }
    }
}

// Main program
int main() {
    void *buffers[NUM_BUFFERS];
    int current_buffer = 0;

    // Allocate screen buffers
    buffers[0] = Physbase();  // Use the physical screen as the first buffer
    buffers[1] = malloc(SCREEN_SIZE);  // Allocate memory for the second buffer
    if (!buffers[1]) {
        return -1;  // Failed to allocate memory
    }

    // Clear both buffers
    clear_screen(buffers[0]);
    clear_screen(buffers[1]);

    int frame = 0;
    while (1) {  // Continue until a key is pressed
        // Draw the next frame into the off-screen buffer
        draw_frame(buffers[1 - current_buffer], frame);

        // Flip buffers by setting the logical screen base
        Setscreen(buffers[1 - current_buffer], -1, -1);

        // Swap buffers
        current_buffer = 1 - current_buffer;

        // Increment the frame counter
        frame++;

      getchar();
    }

    // Clean up and restore the screen
    clear_screen(buffers[0]);
    Setscreen(buffers[0], -1, -1);

    free(buffers[1]);  // Free the allocated buffer

    return 0;
}
