// generate by chatgpt
// 直接用 raw_socket 发送特定ICMP报文，效果等同ping命令

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <time.h>

// ICMP 报文头部长度
#define ICMP_HDRLEN 8

// 计算校验和
unsigned short calculate_checksum(unsigned short *paddress, int len) {
    int nleft = len;
    int sum = 0;
    unsigned short *w = paddress;
    unsigned short answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1) {
        *((unsigned char *)&answer) = *((unsigned char *)w);
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    answer = ~sum;

    return answer;
}

int main() {
    struct icmp icmp_hdr;
    struct sockaddr_in dest_in;
    int sockfd;
    char *target_ip = "192.168.1.2";

    // 创建原始 socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    // 填充目标地址信息
    memset(&dest_in, 0, sizeof(struct sockaddr_in));
    dest_in.sin_family = AF_INET;
    inet_pton(AF_INET, target_ip, &dest_in.sin_addr);

    // 填充 ICMP 报文头部
    icmp_hdr.icmp_type = ICMP_ECHO;
    icmp_hdr.icmp_code = 0;
    icmp_hdr.icmp_id = htons(1000);
    icmp_hdr.icmp_seq = htons(0);
    icmp_hdr.icmp_cksum = 0;
    icmp_hdr.icmp_cksum = calculate_checksum((unsigned short *)&icmp_hdr, ICMP_HDRLEN);

    // 发送 ICMP 报文
    if (sendto(sockfd, &icmp_hdr, ICMP_HDRLEN, 0, (struct sockaddr *)&dest_in, sizeof(dest_in)) <= 0) {
        perror("sendto");
        return 1;
    }

    printf("ICMP echo request sent to %s\n", target_ip);

    // 关闭 socket
    close(sockfd);

    return 0;
}

