#include <stdio.h>
#include <osbind.h>

void main()
{

    void *addr = Physbase();

    printf("Physbase: %p\n", addr);
    getchar();
}
