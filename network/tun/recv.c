// generate by chatgpt
// 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/icmp.h>

#define TUN_DEVICE "/dev/net/tun"
#define BUFSIZE 2000

int tun_alloc(char *dev) {
    struct ifreq ifr;
    int fd, err;

    if ((fd = open(TUN_DEVICE, O_RDWR)) < 0) {
        perror("Opening TUN_DEVICE");
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI; // TUN device, no packet info
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
        if ((i + 1) %16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void handle_icmp_echo_request(int tun_fd, char *buffer, int len) {
    struct iphdr *iph = (struct iphdr *)buffer;
    struct icmphdr *icmph = (struct icmphdr *)(buffer + sizeof(struct iphdr));

    // Change the ICMP type to Echo Reply
    icmph->type = ICMP_ECHOREPLY;
    icmph->checksum = 0;
    icmph->checksum = checksum(icmph, len - sizeof(struct iphdr));

    // Swap source and destination IP addresses
    unsigned int tmp = iph->saddr;
    iph->saddr = iph->daddr;
    iph->daddr = tmp;

    // Recalculate the IP header checksum
    iph->check = 0;
    iph->check = checksum(iph, sizeof(struct iphdr));

    // Send the packet back
    write(tun_fd, buffer, len);
}

int main() {
    char tun_name[IFNAMSIZ] = "tun0";
    int tun_fd, nread;
    char buffer[BUFSIZE];

    tun_fd = tun_alloc(tun_name);
    if (tun_fd < 0) {
        fprintf(stderr, "Error connecting to TUN device %s\n", tun_name);
        return 1;
    }

    printf("Listening on TUN device %s\n", tun_name);

    while (1) {
        nread = read(tun_fd, buffer, BUFSIZE);
        if (nread < 0) {
            perror("Reading from TUN device");
            close(tun_fd);
            return 1;
        }

        printf("Received packet:\n");
        print_packet((unsigned char *)buffer, nread);

        struct iphdr *iph = (struct iphdr *)buffer;
        if (iph->protocol == IPPROTO_ICMP) {
            struct icmphdr *icmph = (struct icmphdr *)(buffer + sizeof(struct iphdr));
            if (icmph->type == ICMP_ECHO) {
                printf("ICMP Echo request received.\n");
                handle_icmp_echo_request(tun_fd, buffer, nread);
                printf("ICMP Echo reply sent.\n");
            }
        }
    }

    close(tun_fd);
    return 0;
}

