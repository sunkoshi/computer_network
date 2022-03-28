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
void printPayload(struct ip *iph, unsigned char *tempBuff)
{
    cout << "Payload: ";
    cout << tempBuff + iph->ip_hl * 4 << endl;
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
        memset(buffer, 0, MAXLINE);
    }
    return 0;
}
