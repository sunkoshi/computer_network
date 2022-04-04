// it will have udp and tcp both
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
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
using namespace std;
#define MAXLINE 1024
#define MSG_CONFIRM 0x800
int udpsfd;
vector<string> addrs;
int UDPServer(int PORT)
{
    int sfd;
    struct sockaddr_in addr, clientAddr;
    int len = sizeof(clientAddr);
    if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(addr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(sfd, (const struct sockaddr *)&addr,
             sizeof(addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return sfd;
}
int SocketServer(int PORT)
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(sfd, (struct sockaddr *)&address, sizeof(address));
    cout << "Binded socket to address.." << endl;
    listen(sfd, SOMAXCONN);
    return sfd;
}
void *HandleTCPClient(void *arg)
{
    int nsfd = *(int *)arg;

    while (true)
    {
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

void *FreeSMS(void *arg)
{
    while (true)
    {
        SendToUDP("127.0.0.1", 5000, "1Free advertisement...");
        sleep(2);
    }
}

void *ListenToPs(void *arg)
{
    struct sockaddr_in addr, clientAddr;
    int len = sizeof(clientAddr);
    memset(&clientAddr, 0, sizeof(clientAddr));

    cout << "UDP listener for Ps is ready...\n";
    while (true)
    {
        char buffer[MAXLINE];
        int n = recvfrom(udpsfd, buffer, MAXLINE,
                         0, (sockaddr *)&clientAddr,
                         (socklen_t *)&len);
        if (n <= 0)
        {
            continue;
        }
        cout << "NC is sending: " << buffer << endl;
    }
}
int main()
{

    cout << "Enter P numebr: ";
    int n;
    cin >> n;

    string config;
    config.push_back('0');
    config += to_string(n);
    SendToUDP("127.0.0.1", 5000, config);

    // listen for Ps
    // pthread_t id;
    // pthread_create(&id, NULL, ListenToPs, NULL);
    // pthread_join(id, NULL);

    int psfd;
    struct sockaddr_in addr, clientAddr;
    int len = sizeof(clientAddr);
    if ((psfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(addr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8085);

    if (bind(psfd, (const struct sockaddr *)&addr,
             sizeof(addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        char buffer[MAXLINE];
        // this will not only rec message but also the client addr
        int n = recvfrom(psfd, (char *)buffer, MAXLINE,
                         0, (sockaddr *)&clientAddr,
                         (socklen_t *)&len);
        if (n <= 0)
        {
            continue;
        }

        cout << "Request Came: " << buffer << endl;
    }

    return 0;
}
