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
#include <bits/stdc++.h>
#include <poll.h>
using namespace std;
void HandleS3(int nsfd)
{
    cout << "Handling in S3..." << endl;
    int valread;
    char buffer[1024] = {0};
    while (1)
    {
        valread = read(nsfd, buffer, 1024);
        string su(buffer);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            break;
            close(nsfd);
        }

        cout << "Client sent: " << buffer << endl;
        memset(buffer, 0, 1024);
        su = "Server: ok";
        strcpy(buffer, su.c_str());
        send(nsfd, buffer, su.size(), 0);
        memset(buffer, 0, 1024);
    }
}

void HandleS1(int nsfd)
{
    cout << "Handling in S1..." << endl;
    int valread;
    char buffer[1024] = {0};
    while (1)
    {
        valread = read(nsfd, buffer, 1024);
        string su(buffer);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            break;
            close(nsfd);
        }

        cout << "Client sent: " << buffer << endl;
        memset(buffer, 0, 1024);
        transform(su.begin(), su.end(), su.begin(), ::toupper);
        su = "From capitalizer server: " + su;
        strcpy(buffer, su.c_str());
        send(nsfd, buffer, su.size(), 0);
        memset(buffer, 0, 1024);
    }
}

void HandleS2(int nsfd)
{
    cout << "Handling in S2..." << endl;
    int valread;
    char buffer[1024] = {0};
    while (1)
    {
        valread = read(nsfd, buffer, 1024);
        string su(buffer);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            break;
            close(nsfd);
        }

        cout << "Client sent: " << buffer << endl;
        memset(buffer, 0, 1024);
        transform(su.begin(), su.end(), su.begin(), ::tolower);
        su = "From lowerizer server: " + su;
        strcpy(buffer, su.c_str());
        send(nsfd, buffer, su.size(), 0);
        memset(buffer, 0, 1024);
    }
}
void HandleServices(int nsfd)
{
    int valread;
    char c;
    valread = read(nsfd, &c, 1);
    if (valread == 0)
    {
        cout << "Client disconnected" << endl;
        close(nsfd);
        return;
    }
    int rIndex = c;
    cout << "Service requested: " << rIndex << endl;
    if (rIndex == 1)
    {
        HandleS1(nsfd);
    }
    else if (rIndex == 2)
    {
        HandleS2(nsfd);
    }
    else
    {
        HandleS3(nsfd);
    }
}
int main()
{
    int PORT = 8888;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(sfd, (struct sockaddr *)&address, sizeof(address));
    listen(sfd, SOMAXCONN);
    for (int i = 0; i < 2; i++)
    {
        struct sockaddr_in cliaddr;
        int addrlen = sizeof(cliaddr);
        cout << "Waiting for connections..." << endl;
        int nsfd = accept(sfd, (struct sockaddr *)&cliaddr, (socklen_t *)&addrlen);
        if (nsfd < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        cout << "New client connected..." << endl;
        HandleServices(nsfd);
        cout << "------------------" << endl;
    }
    return 0;
}