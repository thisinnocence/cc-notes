#include <stdio.h>

int main() {
    printf("Feature status:\n");

#ifdef CONFIG_FEATURE_A
    printf("Feature A is enabled\n");
#else
    printf("Feature A is disabled\n");
#endif

#ifdef CONFIG_FEATURE_B
    printf("Feature B is enabled\n");
#else
    printf("Feature B is disabled\n");
#endif

    return 0;
}
