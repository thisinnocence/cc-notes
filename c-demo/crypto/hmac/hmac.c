// generate by ChatGPT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

/**
HMAC（Hash-based Message Authentication Code）是一种用于生成消息认证码的算法。它结合了散列函数和密钥，用于验证消息的完整性和身份。以下是HMAC的流程：

1. 选择一个合适的散列函数：首先，需要选择一个适合的散列函数，如SHA-1、SHA-256等。
2. 准备密钥：准备一个密钥，该密钥是一个随机的、与消息发送方和接收方共享的秘密值。
3. 对密钥进行处理：如果密钥长度超过散列函数的块大小，则对密钥进行散列运算，以获得与散列函数块大小相同的值。
4. 进行填充：如果密钥长度小于散列函数的块大小，则对密钥进行填充，使其长度与散列函数块大小相同。填充可以是在密钥的右侧附加零字节，或者是对密钥进行散列运算得到更长的密钥。
5. 生成内部密钥：使用填充后的密钥，生成两个内部密钥，分别用于内部散列和外部散列。
6. 进行内部散列：对待认证的消息（或数据）与内部密钥进行散列运算，生成一个中间结果。
7. 进行外部散列：将中间结果与外部密钥进行散列运算，生成最终的消息认证码。
8. 输出结果：输出生成的消息认证码作为消息的完整性和身份验证标志。

HMAC的安全性依赖于散列函数的安全性和密钥的保密性。通过使用密钥，HMAC提供了对消息进行认证和完整性保护的机制
因为只有知道共享密钥的参与方才能正确计算出正确的消息认证码,因此，HMAC可用于确保数据在传输过程中没有被篡改或冒充，并提供一种简单而有效的消息认证机制。
*/

void generateHMAC(const unsigned char* key, int keyLength, const unsigned char* message, int messageLength, unsigned char* hmac) {
    unsigned int hmacLength;
    HMAC(EVP_sha256(), key, keyLength, message, messageLength, hmac, &hmacLength);
}

int main() {
    unsigned char key[] = "my-secret-key";  // 密钥
    unsigned char message[] = "Hello, HMAC!";  // 待认证的消息

    unsigned char hmac[SHA256_DIGEST_LENGTH];  // HMAC结果存储的数组

    generateHMAC(key, strlen((char*)key), message, strlen((char*)message), hmac);

    printf("HMAC-SHA256: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hmac[i]);
    }
    printf("\n");

    return 0;
}

