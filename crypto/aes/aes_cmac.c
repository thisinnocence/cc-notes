#include <openssl/evp.h>
#include <openssl/cmac.h>
#include <stdint.h>

void generateCMAC(const uint8_t *key, const uint8_t *data, size_t dataLen, uint8_t *cmac) {
    CMAC_CTX* ctx = CMAC_CTX_new();

    // 初始化CMAC上下文
    CMAC_Init(ctx, key, 16, EVP_aes_128_cbc(), NULL);

    // 更新数据
    CMAC_Update(ctx, data, dataLen);

    // 完成计算并获取CMAC值
    size_t cmacLen;
    CMAC_Final(ctx, cmac, &cmacLen);

    // 释放CMAC上下文
    CMAC_CTX_free(ctx);
}

int main() {
    uint8_t key[16] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };

    uint8_t data[] = "Hello, AES-CMAC!";
    size_t dataLen = sizeof(data) - 1;  // 不包括字符串结尾的空字符

    uint8_t cmac[EVP_MAX_BLOCK_LENGTH];

    generateCMAC(key, data, dataLen, cmac);

    printf("CMAC: ");
    for (size_t i = 0; i < EVP_MAX_BLOCK_LENGTH; i++) {
        printf("%02x", cmac[i]);
    }
    printf("\n");

    return 0;
}

