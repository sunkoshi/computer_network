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
void printPayload(struct ip *iph, unsigned char *tempBuff, int packetSize)
{
    cout << "Payload: ";
    unsigned short ip_head_len = iph->ip_hl * 4;
    struct tcphdr *tcp_head = (struct tcphdr *)(tempBuff + ip_head_len);
    unsigned char *buf = tempBuff + ip_head_len + tcp_head->th_off * 4;
    size_t length = (packetSize - tcp_head->th_off * 4 - iph->ip_hl * 4);

    for (size_t i = 0; i < length; i++)
    {
        if (i != 0 && i % 16 == 0)
        {
            for (size_t j = (i - 16); j < i; j++)
            {
                if (buf[j] >= 32 && buf[j] <= 128)
                {
                }
            }
            printf("\n");
        }

        cout << buf[i];

        if (i == (length - 1))
        {
            printf("\n");
        }
    }
}
void printIPHeader(struct ip *ip)
{
    cout << "------------------------\n";
    cout << "Printing IP header....\n";
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
    cout << "End of IP header\n";
    cout << "------------------------\n";
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
    int one = 1;
    const int *val = &one;
    if (setsockopt(sfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
    {
        cout << "set socket opt not set";
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
        printPayload(iph, tempBuff, packetSize);
        memset(buffer, 0, MAXLINE);
    }
    return 0;
}
