// generate by chatgpt
// 第一次生成
//   1 没有考虑ping tap0 某个网段IP，首先触发ARP报文
//   2 解析报文没有考虑MAC层，直接从L3开始
// 提示后，chatgpt都很好解决了

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_tun.h>
#include <linux/if.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>

#define TAP_DEVICE "/dev/net/tun"
#define BUFSIZE 2000

int tun_alloc(char *dev) {
    struct ifreq ifr;
    int fd, err;

    if ((fd = open(TAP_DEVICE, O_RDWR)) < 0) {
        perror("Opening TAP_DEVICE");
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);
    return fd;
}

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

void print_packet(const unsigned char *packet, int len) {
    for (int i = 0; i < len; i++) {
        printf("%02x ", packet[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void handle_arp_request(int tap_fd, struct ethhdr *eth, struct arphdr *arp) {
    unsigned char *sender_mac = (unsigned char *)(arp + 1);
    unsigned char *sender_ip = sender_mac + 6;
    unsigned char *target_mac = sender_ip + 4;
    unsigned char *target_ip = target_mac + 6;

    printf("Received ARP request:\n");
    print_packet((unsigned char *)arp, sizeof(struct arphdr) + 20);

    // Prepare ARP reply
    struct ethhdr eth_reply;
    struct arphdr arp_reply;
    unsigned char reply[42];

    // Ethernet header
    memcpy(eth_reply.h_dest, eth->h_source, 6);
    memcpy(eth_reply.h_source, eth->h_dest, 6);
    eth_reply.h_proto = htons(ETH_P_ARP);
    memcpy(reply, &eth_reply, 14);

    // ARP header
    arp_reply.ar_hrd = htons(ARPHRD_ETHER);
    arp_reply.ar_pro = htons(ETH_P_IP);
    arp_reply.ar_hln = 6;
    arp_reply.ar_pln = 4;
    arp_reply.ar_op = htons(ARPOP_REPLY);
    memcpy(reply + 14, &arp_reply, sizeof(struct arphdr));

    // ARP payload
    memcpy(reply + 14 + sizeof(struct arphdr), eth_reply.h_source, 6); // Sender MAC
    memcpy(reply + 20 + sizeof(struct arphdr), target_ip, 4);          // Sender IP
    memcpy(reply + 24 + sizeof(struct arphdr), sender_mac, 6);          // Target MAC
    memcpy(reply + 30 + sizeof(struct arphdr), sender_ip, 4);           // Target IP

    printf("Sending ARP reply:\n");
    print_packet(reply, 42);

    // Send ARP reply
    write(tap_fd, reply, 42);
}

int main() {
    char tap_name[IFNAMSIZ] = "tap0";
    int tap_fd, nread;
    char buffer[BUFSIZE];

    tap_fd = tun_alloc(tap_name);
    if (tap_fd < 0) {
        fprintf(stderr, "Error connecting to TAP device %s\n", tap_name);
        return 1;
    }

    printf("Listening on TAP device %s\n", tap_name);

    while (1) {
        nread = read(tap_fd, buffer, BUFSIZE);
        if (nread < 0) {
            perror("Reading from TAP device");
            close(tap_fd);
            return 1;
        }

        printf("\n");
        for (int i = 0; i < nread; i++) {
            printf("%02x", buffer[i]);
        }
        printf("\n");

        struct ethhdr *eth = (struct ethhdr *)buffer;

        // Handle ARP requests
        if (ntohs(eth->h_proto) == ETH_P_ARP) {
            struct arphdr *arp = (struct arphdr *)(buffer + sizeof(struct ethhdr));
            if (ntohs(arp->ar_op) == ARPOP_REQUEST) {
                handle_arp_request(tap_fd, eth, arp);
            }
        }

        // Handle ICMP Echo requests
        if (ntohs(eth->h_proto) == ETH_P_IP) {
            struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
            if (iph->protocol == IPPROTO_ICMP) {
                struct icmphdr *icmph = (struct icmphdr *)(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));
                if (icmph->type == ICMP_ECHO) {
                    printf("Received ICMP Echo request:\n");
                    print_packet((unsigned char *)iph, nread - sizeof(struct ethhdr));

                    // Change the ICMP type to Echo Reply
                    icmph->type = ICMP_ECHOREPLY;
                    icmph->checksum = 0;
                    icmph->checksum = checksum(icmph, sizeof(struct icmphdr));

                    // Swap source and destination IP addresses
                    struct in_addr saddr;
                    struct in_addr daddr;
                    saddr.s_addr = iph->saddr;
                    daddr.s_addr = iph->daddr;
                    iph->saddr = daddr.s_addr;
                    iph->daddr = saddr.s_addr;

                    // Recalculate the IP header checksum
                    iph->check = 0;
                    iph->check = checksum(iph, sizeof(struct iphdr));

                    printf("Sending ICMP Echo reply:\n");
                    print_packet((unsigned char *)iph, nread - sizeof(struct ethhdr));

                    // Send the packet back
                    write(tap_fd, buffer, nread);
                }
            }
        }
    }

    close(tap_fd);
    return 0;
}
