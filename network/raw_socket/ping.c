/** generate by chatgpt-4o
 *
 * raw_socket 可以直接访问IP头部及以上的数据
 *   适合用于网络协议实现
 *   数据包不会被操作系统的TCP/IP协议栈处理
 *   由于能够直接访问底层网络，原始套接字通常需要root权限
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>           // getpid
#include <arpa/inet.h>        // inet_ntoa
#include <netinet/ip_icmp.h>  // struct icmphdr
#include <sys/socket.h>       // struct socket
#include <netdb.h>            // gethostbyname

// 计算校验和
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main() {
    int sockfd;
    struct sockaddr_in dest_addr;
    struct sockaddr_in src_addr;
    socklen_t addr_len;
    char send_packet[64];
    char recv_packet[1024];
    struct icmphdr *icmp_hdr = (struct icmphdr *)send_packet;
    struct iphdr *ip_hdr;
    struct icmphdr *recv_icmp_hdr;
    struct hostent *host;
    const char *hostname = "www.baidu.com";

    // 解析目标主机名
    host = gethostbyname(hostname);
    if (host == NULL) {
        perror("gethostbyname error");
        return 1;
    }

    // 创建 raw_socket
    // AF_NET: Address Family: Internet, IPv4
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket error");
        return 1;
    }

    // 设置目标地址
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr = *((struct in_addr *)host->h_addr);

    // 构造ICMP报文
    memset(send_packet, 0, sizeof(send_packet));
    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->un.echo.id = getpid();
    icmp_hdr->un.echo.sequence = 1;
    icmp_hdr->checksum = checksum(send_packet, sizeof(send_packet));

    // 发送ICMP报文
    if (sendto(sockfd, send_packet, sizeof(send_packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0) {
        perror("sendto error");
        return 1;
    }

    printf("ICMP Echo Request sent to %s\n", hostname);

    // 接收ICMP响应
    addr_len = sizeof(src_addr);
    if (recvfrom(sockfd, recv_packet, sizeof(recv_packet), 0, (struct sockaddr *)&src_addr, &addr_len) <= 0) {
        perror("recvfrom error");
        return 1;
    }

    // 解析IP头部和ICMP头部
    ip_hdr = (struct iphdr *)recv_packet;
    recv_icmp_hdr = (struct icmphdr *)(recv_packet + (ip_hdr->ihl * 4));

    if (recv_icmp_hdr->type == ICMP_ECHOREPLY && recv_icmp_hdr->un.echo.id == getpid()) {
        // inet_ntoa 返回值在一个 statically allocated buffer, which subsequent calls will overwrite.
        // 也不是thread safe
        printf("ICMP Echo Reply received from %s\n", inet_ntoa(src_addr.sin_addr));
    } else {
        printf("Received packet is not an ICMP Echo Reply\n");
    }

    close(sockfd);
    return 0;
}

