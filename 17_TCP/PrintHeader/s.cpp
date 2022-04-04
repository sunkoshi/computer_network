#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/tcp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <bits/stdc++.h>
using namespace std;
#define PORT 8080
#define MAXLINE 4096
// #define ETH_ALEN 6
// #define ETH_HLAN 14
// #define ETH_ZLEN 60
// #define ETH_DATA_LEN 1500
// #define ETH_FRAME_LEN 1514
// #define ETH_FCS_LEN 4
// #define ETH_P_IP 0x0800
// #define ETH_P_IPV6 0x86DD
// #define ETH_P_ARP 0x0806
// #define ETH_P_PAE 0x888E

// struct ethhdr
// {
//     unsigned char h_dest[ETH_ALEN];
//     unsigned char h_source[ETH_ALEN];
//     unsigned short h_proto;
// } __attribute__((packed));
void printPayload(struct ip *iph, unsigned char *tempBuff)
{
    cout << "--------------PAYLOAD---------------\n";
    cout << tempBuff + iph->ip_hl * 4 << endl;
    cout << endl;
}
void print_tcp_packet(const u_char *Buffer)
{

    unsigned short iphdrlen;
    struct ip *iph = (struct ip *)(Buffer);
    iphdrlen = iph->ip_hl * 4;
    struct tcphdr *tcph = (struct tcphdr *)(Buffer + iphdrlen);

    printf("TCP Header\n");
    printf("   |-Source Port      : %u\n", ntohs(tcph->th_sport));
    printf("   |-Destination Port : %u\n", ntohs(tcph->th_dport));
    printf("   |-Sequence Number    : %u\n", ntohl(tcph->th_seq));
    printf("   |-Acknowledge Number : %u\n", ntohl(tcph->th_ack));
    printf("   |-Header Length      : %d DWORDS or %d BYTES\n", (unsigned int)tcph->th_off, (unsigned int)tcph->th_off * 4);
    cout << "   |-Flag   : " << tcph->th_x2 << endl;
    // printf("   |-Urgent Flag          : %d\n", (unsigned int)tcph->);
    // printf("   |-Acknowledgement Flag : %d\n", (unsigned int)tcph->ack);
    // printf("   |-Push Flag            : %d\n", (unsigned int)tcph->psh);
    // printf("   |-Reset Flag           : %d\n", (unsigned int)tcph->rst);
    // printf("   |-Synchronise Flag     : %d\n", (unsigned int)tcph->syn);
    // printf("   |-Finish Flag          : %d\n", (unsigned int)tcph->fin);
    printf("   |-Window         : %d\n", ntohs(tcph->th_win));
    printf("   |-Checksum       : %d\n", ntohs(tcph->th_sum));
    printf("   |-Urgent Pointer : %d\n", tcph->th_urp);
}

void printIPHeader(struct ip *ip)
{
    cout << "------------IP HEADER------------\n";
    cout << "IP version:" << (unsigned int)ip->ip_v << endl;
    cout << "IP header length:" << (unsigned int)ip->ip_hl << endl;
    cout << "Type of service:" << (unsigned int)ip->ip_tos << endl;
    cout << "Total ip packet length:" << ntohs(ip->ip_len) << endl;
    cout << "Packet id:" << ntohs(ip->ip_id) << endl;
    cout << "Time to leave :" << (unsigned int)ip->ip_ttl << endl;
    cout << "Protocol:" << (unsigned int)ip->ip_p << endl;
    cout << "Check:" << ip->ip_sum << endl;
    cout << "Source ip:" << inet_ntoa(*(in_addr *)&ip->ip_src) << endl;
    cout << "Destination ip:" << inet_ntoa(*(in_addr *)&ip->ip_dst) << endl;
    cout << endl;
}

int main()
{
    int sfd;
    struct sockaddr_in addr, clientAddr;
    int len = sizeof(clientAddr);
    if ((sfd = socket(PF_INET, SOCK_RAW, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(addr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sfd, (const struct sockaddr *)&addr,
         sizeof(addr));

    while (true)
    {
        unsigned char buffer[MAXLINE];
        // this will not only rec message but also the client add
        int packetSize = recvfrom(sfd, (char *)buffer, MAXLINE,
                                  0, (sockaddr *)&clientAddr,
                                  (socklen_t *)&len);
        if (packetSize <= 0)
        {
            continue;
        }
        unsigned char tempBuff[MAXLINE];
        for (int i = 0; i < MAXLINE; i++)
            tempBuff[i] = buffer[i];
        struct ip *iph = (struct ip *)buffer;
        printIPHeader(iph);
        printPayload(iph, tempBuff);
        cout << endl;
        cout << "-------------TCP------------\n";
        print_tcp_packet(buffer);
        memset(buffer, 0, MAXLINE);
    }
    return 0;
}
