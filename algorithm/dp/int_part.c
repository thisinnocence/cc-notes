#include <stdio.h>

unsigned int get_int_part(int n, int max) {
    if (n == 1 || max == 1)
        return 1;

    if (n < max)
        return get_int_part(n, n);

    if (n == max)
        return 1 + get_int_part(n, n - 1);

    return get_int_part(n - max, max) + get_int_part(n, max - 1);
}

unsigned int dp_int_part(int n, int max) {
    int i, j;
    unsigned int table[200][200] = {0};
    for (i = 1; i <= n; i++) {
        for (j = 1; j <= i; j++) {
            if (i == 1 || j == 1) {
                table[i][j] = 1;
            } else {
                if (i == j) {
                    table[i][j] = table[i][j - 1] + 1;
                } else if (i - j < j) {
                    table[i][j] = table[i - j][i - j] + table[i][j - 1];
                } else {
                    table[i][j] = table[i - j][j] + table[i][j - 1];
                }
            }
        }
    }
    return table[n][max];
}

int main() {
    int n = 0;
    while (n != -1) {
        scanf("%d", &n);
        printf("%u %u\n", get_int_part(n, n), dp_int_part(n, n));
    }
    return 0;
}

