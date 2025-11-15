#include <stdio.h>

#include "main.h"
#include "math/add.h"

int main(void)
{
    printf("add 1 + 2 = %d\n", add(1, 2));
    printf("MAIN_ID is %d\n", MAIN_ID);
    return 0;
}
