#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>

// AES加密函数
void encryptAES(const unsigned char* plaintext, int plaintextLength, const unsigned char* key, unsigned char* ciphertext) {
    AES_KEY aesKey;
    AES_set_encrypt_key(key, 128, &aesKey);

    // 分块加密
    int numBlocks = plaintextLength / AES_BLOCK_SIZE;
    for (int i = 0; i < numBlocks; i++) {
        AES_encrypt(plaintext + (i * AES_BLOCK_SIZE), ciphertext + (i * AES_BLOCK_SIZE), &aesKey);
    }

    // 处理最后不足一个块的部分
    int remainingBytes = plaintextLength % AES_BLOCK_SIZE;
    if (remainingBytes != 0) {
        unsigned char paddedBlock[AES_BLOCK_SIZE];
        memset(paddedBlock, 0, sizeof(paddedBlock));
        memcpy(paddedBlock, plaintext + (numBlocks * AES_BLOCK_SIZE), remainingBytes);
        AES_encrypt(paddedBlock, ciphertext + (numBlocks * AES_BLOCK_SIZE), &aesKey);
    }
}

// AES解密函数
void decryptAES(const unsigned char* ciphertext, int ciphertextLength, const unsigned char* key, unsigned char* plaintext) {
    AES_KEY aesKey;
    AES_set_decrypt_key(key, 128, &aesKey);

    // 分块解密
    int numBlocks = ciphertextLength / AES_BLOCK_SIZE;
    for (int i = 0; i < numBlocks; i++) {
        AES_decrypt(ciphertext + (i * AES_BLOCK_SIZE), plaintext + (i * AES_BLOCK_SIZE), &aesKey);
    }

    // 处理最后不足一个块的部分
    int remainingBytes = ciphertextLength % AES_BLOCK_SIZE;
    if (remainingBytes != 0) {
        unsigned char paddedBlock[AES_BLOCK_SIZE];
        memset(paddedBlock, 0, sizeof(paddedBlock));
        AES_decrypt(ciphertext + (numBlocks * AES_BLOCK_SIZE), paddedBlock, &aesKey);
        memcpy(plaintext + (numBlocks * AES_BLOCK_SIZE), paddedBlock, remainingBytes);
    }
}

int main() {
    unsigned char plaintext[] = "This is a sample plaintext to be encrypted using AES.";
    unsigned char key[] = "0123456789abcdef"; // AES-128密钥，16字节
    int plaintextLength = strlen((char*)plaintext);
    int ciphertextLength = (plaintextLength / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;

    unsigned char* ciphertext = (unsigned char*)malloc(ciphertextLength);
    unsigned char* decryptedPlaintext = (unsigned char*)malloc(ciphertextLength);

    // 加密
    encryptAES(plaintext, plaintextLength, key, ciphertext);

    // 解密
    decryptAES(ciphertext, ciphertextLength, key, decryptedPlaintext);

    printf("Plaintext: %s\n", plaintext);
    printf("Ciphertext (hex): ");
    for (int i = 0; i < ciphertextLength; i++) {
        printf("%02x", ciphertext[i]);
    }
    printf("\n");
    printf("Decrypted plaintext: %s\n", decryptedPlaintext);

    free(ciphertext);
    free(decryptedPlaintext);

    return 0;
}

