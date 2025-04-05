#include <stdio.h>
#include <math.h>

int main() {
    float pi = 3.1415926;
    double re = round(pi * 100) / 100;
    // %[flags][width][.precision][length]specifier
    // For a, A, e, E, f and F specifiers: this is the number of digits to be printed 
    //   after the decimal point (by default, this is 6).
    //   f: Decimal floating point, lowercase
    printf("%.2f\n", re); // output: 3.14
    return 0;
}
