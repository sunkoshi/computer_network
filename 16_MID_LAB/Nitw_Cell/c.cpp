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
#define MAXLINE 4096
int udpsfd;
void printPayload(struct ip *iph, unsigned char *tempBuff)
{
    cout << "Free SMS: ";
    cout << tempBuff + iph->ip_hl * 4 << endl;
}
int WriteFD(int fd, string s)
{
    s.push_back('\0');
    return send(fd, s.c_str(), s.size(), 0);
}
string ReadFD(int fd)
{
    string s = "";
    char ch;
    while (true)
    {
        int n = read(fd, &ch, 1);
        if (n <= 0 || ch == '\0')
        {
            break;
        }
        s.push_back(ch);
    }
    return s;
}
int TCPConnWithNC()
{
    // socket client
    int PORT = 3000;
    int sfd, valread;
    struct sockaddr_in address;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    while (connect(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect");
        cout << "Connection failed, retrying in 5 seconds" << endl;
        sleep(5);
    }
    cout << "Connection established with NC1...\n";
    return sfd;
}

void *ListenToFreeSMS(void *arg)
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
    addr.sin_port = htons(8081);
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
        printPayload(iph, tempBuff);
        memset(buffer, 0, MAXLINE);
    }
}
void SendToUDP(string ip, int PORT, string s)
{
    struct sockaddr_in addr;
    if ((udpsfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
        memset(&addr, 0, sizeof(addr));
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    s.push_back('\0');
    sendto(udpsfd, s.c_str(), s.size(),
           0, (const struct sockaddr *)&addr, sizeof(addr));
}
int main()
{

    int sfd = TCPConnWithNC();
    cout << "Enter package to take service from: ";
    char pkg;
    cin >> pkg;
    char ch;
    cout << "Select option: 1-FreeSMS, 2-PaidSMS, 3-Live: ";
    char option;
    cin >> option;

    string config;
    config.push_back(pkg);
    config.push_back(option);
    WriteFD(sfd, config);

    if (option == '1')
    {
        pthread_t id;
        cout << "Listening to free sms...\n";
        pthread_create(&id, NULL, ListenToFreeSMS, NULL);
        pthread_join(id, NULL);
    }
    else
    {
    }
    WriteFD(sfd, config);
    string ipOfP = ReadFD(sfd);
    cout << ipOfP;

    int psfd;
    struct sockaddr_in addr;
    if ((psfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
        memset(&addr, 0, sizeof(addr));
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8085);
    if (inet_pton(AF_INET, ipOfP.c_str(), &addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    string s = "C2Msg=Hi";
    sendto(psfd, s.c_str(), s.size(),
           0, (const struct sockaddr *)&addr, sizeof(addr));

    close(sfd);
    return 0;
}
