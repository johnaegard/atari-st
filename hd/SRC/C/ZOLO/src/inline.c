#include <stdio.h>

unsigned int add_numbers(unsigned int a, unsigned int b) {
  unsigned int result;
  __asm__ volatile (
    "add.l %1, %0"  // ADD.L (Long-word addition)
    : "=d" (result) // Output: result in a data register
    : "d" (b), "0" (a) // Inputs: a and b
    );
  return result;
}

int main() {
  unsigned int sum = add_numbers(10, 20);
  printf("Sum: %u\n", sum);
  getchar();
  return 0;
}
