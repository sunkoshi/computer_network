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
int main()
{
    int sfd, nsfd, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // socket file descriptor
    // getting a socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Created socket.." << endl;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    // binding the address to sfd
    if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "Binded socket to address.." << endl;

    // listen to the socket, with maximum buffer for clients
    if (listen(sfd, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    cout << "Socket listening on port " << PORT << endl;

    // accept the incoming connection and get nsfd
    if ((nsfd = accept(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    cout << "Client connected... " << endl;

    while (1)
    {
        unsigned char tempBuff[MAXLINE], buffer[MAXLINE];
        int packetSize = read(nsfd, buffer, 1024);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            break;
        }
        for (int i = 0; i < MAXLINE; i++)
            tempBuff[i] = buffer[i];
        struct ip *iph = (struct ip *)buffer;
        printPayload(iph, tempBuff, packetSize);
        memset(buffer, 0, MAXLINE);
    }

    return 0;
}
