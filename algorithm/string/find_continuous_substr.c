#include <stdio.h>
#include <string.h>

// 15min, just do it quick, quick coding
// FSM mechod, kind of

void find_sub(char *str, char *substr)
{
    char out[10000] = {0};
    char *re = out;
    size_t len = strlen(substr);
    while (*str) {
        char *pos = strstr(str, substr);
        if (pos != NULL) {
            while (str != pos) {
                *re++ = *str++;
            }
            *re++ = '(';
            while (strstr(str, substr) == str) {
                strcpy(re, substr);
                re += len;
                str += len;
            }
            *re++ = ')';
        } else {
            while (*str) {
                *re++ = *str++;
            }
        }
    }
    printf("%s\n", out);
}

int main()
{
    char str1[] = "xxhelloxxhellohelloxxxx";
    char str2[] = "hello";
    find_sub(str1, str2);
    return 0;
}
