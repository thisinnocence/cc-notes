#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "snow-3g.h"

void printBytes(uint8_t *b, uint32_t len)
{
    printf("[%02u] ", len);
    for (uint32_t i = 0; i < len; i++) {
        printf("%02x", b[i]);
    }
    printf("\n");
}

uint8_t hexCharToUint8(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return 0;  // 非法字符，默认返回 0
}

// out: need free by caller
// len: memory len pointed by out
void hex2byte(const char* hex, uint8_t **out, uint32_t *len)
{
    assert(strlen(hex) % 2 == 0);
    *len = strlen(hex)/2;
    *out = (uint8_t*)malloc(*len);
    for(uint32_t i = 0; i < strlen(hex); i+=2) {
        uint8_t high = hexCharToUint8(hex[i]);
        uint8_t low = hexCharToUint8(hex[i+1]);
        (*out)[i/2] = (high << 4) | low;
    }
}

int main()
{
    // gen data
    const char *_data = "3332346263393861373479";
    uint8_t *data;
    uint32_t data_len;
    hex2byte(_data, &data, &data_len);
    printf("data: ");
    printBytes(data, data_len);

    // gen key
    const char *_key = "2bd6459f82c5b300952c49104881ff48";
    uint8_t *key;
    uint32_t key_len;
    hex2byte(_key, &key, &key_len);
    printf(" key: ");
    printBytes(key, key_len);

    // auth ==> mac
    uint8_t mac[4];
    snow_3g_f9(key, 0x38a6f056, ((int64_t)0x1f << 27), 0, data, data_len*8, mac);
    printf(" mac: ");
    printBytes(mac, sizeof(mac));
    
    const char *expect_mac = "731f1165";
    uint8_t *emac;
    uint32_t emac_len;
    hex2byte(expect_mac, &emac, &emac_len);
    printf("emac: ");
    printBytes(emac, emac_len);

    assert(memcmp(mac, emac, 4) == 0);
    free(data);
    free(key);
    free(emac);
}
