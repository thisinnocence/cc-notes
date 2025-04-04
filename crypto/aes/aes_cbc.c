#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>

void handleErrors()
{
    printf("Error occurred.\n");
    exit(1);
}

void aes_cbc_encrypt(const unsigned char *plaintext, int plaintext_len,
                     const unsigned char *key, const unsigned char *iv,
                     unsigned char *ciphertext)
{
    AES_KEY aesKey;
    AES_set_encrypt_key(key, 128, &aesKey);
    AES_cbc_encrypt(plaintext, ciphertext, plaintext_len, &aesKey, iv, AES_ENCRYPT);
}

void aes_cbc_decrypt(const unsigned char *ciphertext, int ciphertext_len,
                     const unsigned char *key, const unsigned char *iv,
                     unsigned char *plaintext)
{
    AES_KEY aesKey;
    AES_set_decrypt_key(key, 128, &aesKey);
    AES_cbc_encrypt(ciphertext, plaintext, ciphertext_len, &aesKey, iv, AES_DECRYPT);
}

int main()
{
    // AES-CBC加密示例

    // 输入明文
    unsigned char plaintext[] = "Hello, AES-CBC!";
    int plaintext_len = strlen((char *)plaintext);

    // 定义密钥和IV
    unsigned char key[] = "0123456789abcdef";
    unsigned char iv[] = "1234567890abcdef";

    // 计算填充后的明文长度
    int padded_len = ((plaintext_len + AES_BLOCK_SIZE) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;

    // 定义加密后的密文
    unsigned char ciphertext[padded_len];

    // 执行加密操作
    aes_cbc_encrypt(plaintext, padded_len, key, iv, ciphertext);

    printf("Plaintext: %s\n", plaintext);
    printf("Ciphertext: ");
    for (int i = 0; i < padded_len; i++)
        printf("%02x", ciphertext[i]);
    printf("\n");

    // AES-CBC解密示例

    // 定义解密后的明文
    unsigned char decrypted_text[padded_len];

    // 执行解密操作
    aes_cbc_decrypt(ciphertext, padded_len, key, iv, decrypted_text);

    printf("Decrypted Text: %s\n", decrypted_text);

    return 0;
}

