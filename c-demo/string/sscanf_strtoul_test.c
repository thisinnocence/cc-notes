#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

uint32_t str2ip_1(const char *ip) {
    uint32_t re;
    char *val = (char *) &re;
    sscanf(ip, "%hhu.%hhu.%hhu.%hhu", &val[3], &val[2], &val[1], &val[0]);
    return re;
}

uint32_t str2ip_2(const char *ip) {
    uint32_t re;
    uint8_t *val = (uint8_t*)&re + 3; // byte-order
    char *next = (char *) ip;
    while (next && *next) {
        *val-- = (uint8_t)strtoul(next, &next, 0);
        while (!isdigit(*next) && *next) next++;
    }
    return re;
}

int main() {
    const char *ip = "192.168.1.1";
    uint32_t val;

    val = str2ip_1(ip);
    printf("%u\n", val);

    val = str2ip_2(ip);
    printf("%u\n", val);

    return 0;
}

/* output:
3232235777
3232235777
*/

