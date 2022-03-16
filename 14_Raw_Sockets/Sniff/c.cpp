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
// #define MAXLINE 4096
// unsigned short csum(unsigned short *buf, int nwords)
// {
//     unsigned long sum;
//     for (sum = 0; nwords > 0; nwords--)
//         sum += *buf++;
//     sum = (sum >> 16) + (sum & 0xffff);
//     sum += (sum >> 16);
//     return ~sum;
// }
// int main()
// {
//     int sfd;
//     struct sockaddr_in addr;
//     memset(&addr, 0, sizeof(addr));
//     if ((sfd = socket(PF_INET, SOCK_RAW, 0)) < 0)
//     {
//         perror("socket creation failed");
//         exit(EXIT_FAILURE);
//     }
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(PORT);
//     addr.sin_addr.s_addr = inet_addr("127.0.0.1");

//     while (true)
//     {
//         string s = "";
//         getline(cin, s);

//         char buffer[MAXLINE] = "hello";
//         struct ipheader *ip = (struct ipheader *)buffer;
//         int one = 1;
//         const int *val = &one;
//         memset(buffer, 0, MAXLINE);

//         struct ip *iph;
//         iph->ip_hl = 5;
//         iph->ip_v = 4;
//         iph->ip_tos = 16;
//         iph->ip_len = sizeof(struct ip);
//         iph->ip_id = htonl(54321);
//         iph->ip_off = 0;
//         iph->ip_ttl = 64;
//         iph->ip_p = 17;
//         struct sockaddr_in fakeIP;
//         fakeIP.sin_addr.s_addr = inet_addr("90.91.92.93");
//         iph->ip_src = fakeIP.sin_addr;

//         iph->ip_dst = addr.sin_addr;
//         iph->ip_sum = csum((unsigned short *)buffer, iph->ip_len);

//         if (setsockopt(sfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
//         {
//             perror("setsockopt() error");
//             exit(-1);
//         }
//         else
//             printf("setsockopt() is OK.\n");

//         sendto(sfd, buffer, iph->ip_len,
//                0, (const struct sockaddr *)&addr, sizeof(addr));
//         memset(buffer, 0, MAXLINE);
//     }

//     close(sfd);
//     return 0;
// }

#define P 25

unsigned short
csum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}

int main(void)
{
    int s = socket(PF_INET, SOCK_RAW, 0);
    char datagram[4096];
    struct ip *iph = (struct ip *)datagram;
    struct tcphdr *tcph = (struct tcphdr *)datagram + sizeof(struct ip);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(datagram, 0, 4096);

    // table data
    iph->ip_hl = 5;
    iph->ip_v = 4;
    iph->ip_tos = 0;
    iph->ip_len = sizeof(struct ip) + sizeof(struct tcphdr);
    iph->ip_id = htonl(54321);
    iph->ip_off = 0;
    iph->ip_ttl = 255;
    iph->ip_p = 6;
    iph->ip_sum = 0;
    iph->ip_src.s_addr = inet_addr("1.2.3.4");
    iph->ip_dst.s_addr = sin.sin_addr.s_addr;
    tcph->th_sport = htons(1234);
    tcph->th_dport = htons(P);
    tcph->th_seq = random();
    tcph->th_ack = 0;
    tcph->th_x2 = 0;
    tcph->th_off = 0;
    tcph->th_flags = TH_SYN;
    tcph->th_win = htonl(65535);
    tcph->th_sum = 0;

    tcph->th_urp = 0;

    iph->ip_sum = csum((unsigned short *)datagram, iph->ip_len >> 1);

    {
        int one = 1;
        const int *val = &one;
        if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
            printf("Warning: Cannot set HDRINCL!\n");
    }

    if (sendto(s,
               datagram,
               iph->ip_len,
               0,
               (struct sockaddr *)&sin,
               sizeof(sin)) < 0)
        printf("error\n");
    else
        printf("done");

    return 0;
}
