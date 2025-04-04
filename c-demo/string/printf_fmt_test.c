#include <stdio.h>

// http://www.cplusplus.com/reference/cstdio/printf/

int main() {
    double pi = 3.1415926;

    // %[flags][width][.precision][length]specifier
    // For a, A, e, E, f and F specifiers: this is the number of digits to be printed
    //   after the decimal point (by default, this is 6).
    // f: Decimal floating point, lowercase
    printf("%f \n", pi); // 3.141593
    printf("%.2f \n", pi); // 3.14

    // Minimum number of characters to be printed. If the value to be printed is shorter than this number,
    //   the result is padded with blank spaces. The value is not truncated even if the result is larger.
    printf("%5.2f \n", pi); // " 3.14"

    // specifier:
    // hh - signed char
    // h  - short int
    // ll - long long int
    unsigned int a = 0x1122aabb;
    unsigned char *p = (unsigned char*)&a;
    printf("%hhx %hhx\n", p[0], p[1]); // bb aa
    printf("%hx %hx\n", (unsigned short)a, (unsigned short)p[0]); // aabb bb


    return 0;
}
