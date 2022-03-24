#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <bits/stdc++.h>
#include <sys/un.h>
#include "my_fh.h"
using namespace std;
#define MAXLINE 4098
void printPayload(struct ip *iph, unsigned char *tempBuff, int packetSize)
{
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

void *ADV(void *arg)
{
    cout << "Advertisement Thread Running..." << endl;
    int PORT = 6000;
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
        cout << "Advertisement: ";
        printPayload(iph, tempBuff, packetSize);
        memset(buffer, 0, MAXLINE);
        cout << endl;
    }
    return 0;
}
int main()
{
    pthread_t id;
    pthread_create(&id, NULL, ADV, NULL);
    int sfd;
    struct sockaddr_un saddr_un;
    char buf[MAXLINE];
    string server_socket_path = "./train.sock";
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&saddr_un, 0, sizeof(sockaddr_un));
    saddr_un.sun_family = AF_UNIX;
    strcpy(saddr_un.sun_path, server_socket_path.c_str());
    connect(sfd, (struct sockaddr *)&saddr_un, sizeof(saddr_un));
    cout << "Connected to Station Master...\n";
    while (1)
    {
        cout << "Waiting for Train...\n";
        int tsfd = RecFD(sfd);
        cout << "Getting train info...\n";
        SendFD(sfd, tsfd);
    }

    return 0;
}