#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main() {
    // Create a raw socket for ICMP protocol
    int sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock_fd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare destination address
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr("8.8.8.8");

    // Prepare ICMP echo request packet
    struct icmphdr icmp_hdr;
    memset(&icmp_hdr, 0, sizeof(icmp_hdr));
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.code = 0;
    icmp_hdr.checksum = htons(0xffff);

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, &icmp_hdr, sizeof(icmp_hdr));

    // Send ICMP echo request packet to destination
    int send_result = sendto(sock_fd, buffer, sizeof(buffer), 0, 
                             (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (send_result < 0) {
        perror("sendto failed");
        exit(EXIT_FAILURE);
    }

    // Wait for ICMP echo reply packet
    char recv_buffer[BUFFER_SIZE];
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);

    int recv_result = recvfrom(sock_fd, recv_buffer, sizeof(recv_buffer), 0,
                               (struct sockaddr *)&recv_addr, &addr_len);
    if (recv_result < 0) {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }

    // Extract ICMP echo reply packet
    struct icmphdr *icmp_reply = (struct icmphdr *)recv_buffer;
    if (icmp_reply->type == ICMP_ECHOREPLY) {
        printf("ICMP Echo Reply received from %s\n", inet_ntoa(recv_addr.sin_addr));
    } else {
        printf("Unknown ICMP packet received from %s\n", inet_ntoa(recv_addr.sin_addr));
    }

    // Close the socket
    close(sock_fd);

    return 0;
}



// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netinet/ip.h>
// #include <netinet/ip_icmp.h>
// #include <arpa/inet.h>

// #define PACKET_SIZE 64

// unsigned short checksum(unsigned short *ptr, int nbytes) {
//     unsigned long sum;
//     unsigned short oddbyte;
//     unsigned short answer;
 
//     sum = 0;
//     while (nbytes > 1) {
//         sum += *ptr++;
//         nbytes -= 2;
//     }
//     if (nbytes == 1) {
//         oddbyte = 0;
//         *((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
//         sum += oddbyte;
//     }
 
//     sum = (sum >> 16) + (sum & 0xffff);
//     sum += (sum >> 16);
//     answer = (unsigned short)~sum;
 
//     return answer;
// }

// int main(int argc, char **argv) {
//     int sockfd;
//     char packet[PACKET_SIZE];
//     struct sockaddr_in dest_addr;
//     struct icmphdr *icmp_hdr;
//     struct timeval start_time, end_time;
//     double rtt;

//     // Check command-line arguments
//     if (argc != 2) {
//         printf("Usage: %s <destination IP>\n", argv[0]);
//         exit(1);
//     }

//     // Create raw socket
//     sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
//     if (sockfd < 0) {
//         perror("socket");
//         exit(1);
//     }

//     // Set destination address
//     memset(&dest_addr, 0, sizeof(dest_addr));
//     dest_addr.sin_family = AF_INET;
//     dest_addr.sin_addr.s_addr = inet_addr(argv[1]);

//     // Prepare ICMP packet
//     memset(packet, 0, PACKET_SIZE);
//     icmp_hdr = (struct icmphdr *)packet;
//     icmp_hdr->type = ICMP_ECHO;
//     icmp_hdr->code = 0;
//     icmp_hdr->checksum = 0;
//     icmp_hdr->un.echo.id = getpid() & 0xFFFF;
//     icmp_hdr->un.echo.sequence = 1;
//     icmp_hdr->checksum = checksum((unsigned short *)icmp_hdr, PACKET_SIZE);

//     // Send ICMP packet
//     if (sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
//         perror("sendto");
//         exit(1);
//     }

//     // Receive ICMP packet
//     while (1) {
//         char recv_buf[PACKET_SIZE];
//         struct sockaddr_in recv_addr;
//         socklen_t recv_addr_len = sizeof(recv_addr);
//         struct iphdr *ip_hdr;
//         struct icmphdr *icmp_hdr;
//         int bytes_received;

//         bytes_received = recvfrom(sockfd, recv_buf, PACKET_SIZE, 0, (struct sockaddr *)&recv_addr, &recv_addr_len);
//         if (bytes_received < 0) {
//             perror("recvfrom");
//             exit(1);
//         }

//         ip_hdr = (struct iphdr *)recv_buf;
//         icmp_hdr = (struct icmphdr *)(recv_buf + (ip_hdr->ihlsizeof(struct udphdr)));
// if (icmp_hdr->type == ICMP_ECHOREPLY) {
// if (icmp_hdr->un.echo.id == (getpid() & 0xFFFF)) {
// gettimeofday(&end_time, NULL);
// rtt = (double)(end_time.tv_usec - start_time.tv_usec) / 1000.0 + (double)(end_time.tv_sec - start_time.tv_sec) * 1000.0;
// printf("Reply from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", inet_ntoa(recv_addr.sin_addr), icmp_hdr->un.echo.sequence, ip_hdr->ttl, rtt);
// break;
// }
// }
// }

// // Close socket
// close(sockfd);

// return 0;
// }