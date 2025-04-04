#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <openssl/aes.h>
#include <assert.h>

void aes_crt_encrypt(const uint8_t *key, const uint8_t *iv, const uint8_t *plaintext, size_t plaintext_length, uint8_t *ciphertext) {
    AES_KEY aes_key;
    AES_set_encrypt_key(key, 128, &aes_key);

    uint8_t counter[AES_BLOCK_SIZE];
    memcpy(counter, iv, AES_BLOCK_SIZE);

    uint8_t encrypted_counter[AES_BLOCK_SIZE];
    uint8_t block[AES_BLOCK_SIZE];

    size_t i, j;
    for (i = 0; i < plaintext_length; i += AES_BLOCK_SIZE) {
        AES_encrypt(counter, encrypted_counter, &aes_key);
        for (j = 0; j < AES_BLOCK_SIZE && i + j < plaintext_length; ++j) {
            block[j] = plaintext[i + j] ^ encrypted_counter[j];
            ciphertext[i + j] = block[j];
        }
        for (j = AES_BLOCK_SIZE - 1; j >= 0; --j) {
            if (++counter[j]) {
                break;
            }
        }
    }
}

void aes_crt_decrypt(const uint8_t *key, const uint8_t *iv, const uint8_t *ciphertext, size_t ciphertext_length, uint8_t *plaintext) {
    aes_crt_encrypt(key, iv, ciphertext, ciphertext_length, plaintext);
}

int main() {
    // 密钥，128位（16字节）
    uint8_t key[AES_BLOCK_SIZE] = {
        0x2b, 0x7e, 0x15, 0x16,
        0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88,
        0x09, 0xcf, 0x4f, 0x3c
    };

    // 初始化向量（IV），128位（16字节）
    uint8_t iv[AES_BLOCK_SIZE] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b,
        0x0c, 0x0d, 0x0e, 0x0f
    };

    // 明文
    const char *plaintext = "Hello, AES-CRT 128!";
    size_t plaintext_length = strlen(plaintext);

    // 密文缓冲区
    uint8_t ciphertext[plaintext_length];

    // 加密
    aes_crt_encrypt(key, iv, (const uint8_t *)plaintext, plaintext_length, ciphertext);

    // 解密
    uint8_t decrypted_text[plaintext_length];
    memset(decrypted_text, 0, plaintext_length);
    aes_crt_decrypt(key, iv, ciphertext, plaintext_length, decrypted_text);

    // 打印结果
    printf("Plaintext: %s\n", plaintext);
    printf("encrypt text: ");
    for (size_t i = 0; i < plaintext_length; ++i) {
        printf("%02x", ciphertext[i]);
        if ((i+1) % 4 == 0) printf(" ");
    }
    printf("\n");
    decrypted_text[plaintext_length] = '\0';
    printf("decrypt text: %s\n", decrypted_text);

    return 0;
}

