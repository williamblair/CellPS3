#include <stdio.h>

/* The first arg is the spe id of the spe thread that issued it */
int main(unsigned long long speid) 
{
    printf("Hello World from SPU (0x%llx)\n", speid);

    return 0;
}
