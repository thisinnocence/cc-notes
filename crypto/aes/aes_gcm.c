#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>

/*
# tag
在AES-GCM模式中，tag是认证标签（Authentication Tag），用于验证加密数据的完整性和真实性。
它是通过对密文进行认证计算得出的结果。

在加密过程中，除了生成密文之外，AES-GCM还会生成一个固定长度的认证标签。这个标签会基于密钥、初始化向量（IV）、明文和密文计算出来。
认证标签通常是一个固定长度的哈希值或MAC（Message Authentication Code）。

在解密过程中，接收者会使用相同的密钥、IV、密文和解密后的明文来重新计算认证标签。
然后，接收者会将计算得到的认证标签与发送方提供的认证标签进行比较。如果两个认证标签匹配，则说明数据在传输过程中没有被篡改或损坏。

*/

void handleErrors()
{
    printf("Error occurred.\n");
    exit(1);
}

void aes_gcm_encrypt(const unsigned char *plaintext, int plaintext_len,
                     const unsigned char *key, const unsigned char *iv,
                     unsigned char *ciphertext, unsigned char *tag)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL))
        handleErrors();

    if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv))
        handleErrors();

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
        handleErrors();

    EVP_CIPHER_CTX_free(ctx);
}

void aes_gcm_decrypt(const unsigned char *ciphertext, int ciphertext_len,
                     const unsigned char *tag, const unsigned char *key,
                     const unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    int ret;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if (!EVP_DecryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL))
        handleErrors();

    if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv))
        handleErrors();

    if (!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, (void *)tag))
        handleErrors();

    ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

    if (ret > 0)
        plaintext_len += len;
    else
        handleErrors();

    EVP_CIPHER_CTX_free(ctx);
}

int main()
{
    // AES-GCM加密示例

    // 输入明文
    unsigned char plaintext[] = "Hello, AES-GCM!";
    int plaintext_len = strlen((char *)plaintext);

    // 定义密钥和IV
    unsigned char key[] = "0123456789abcdef";
    unsigned char iv[] = "1234567890abcdef";

    // 定义加密后的密文和标签
    unsigned char ciphertext[128];
    unsigned char tag[16];

    // 执行加密操作
    aes_gcm_encrypt(plaintext, plaintext_len, key, iv, ciphertext, tag);

    printf("Plaintext: %s\n", plaintext);
    printf("Ciphertext: ");
    for (int i = 0; i < plaintext_len; i++)
        printf("%02x", ciphertext[i]);
    printf("\n");
    printf("Tag: ");
    for (int i = 0; i < sizeof(tag); i++)
        printf("%02x", tag[i]);
    printf("\n");

    // AES-GCM解密示例

    // 定义解密后的明文
    unsigned char decrypted_text[128] = {0};

    // 执行解密操作
    aes_gcm_decrypt(ciphertext, plaintext_len, tag, key, iv, decrypted_text);

    printf("Decrypted Text: %s\n", decrypted_text);

    return 0;
}

