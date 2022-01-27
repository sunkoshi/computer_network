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
#include <arpa/inet.h>
using namespace std;

int main()
{
    // socket client
    int PORT = 8080;
    int sfd, valread;
    struct sockaddr_in address;
    char buffer[1024] = {0};

    // socket file descriptor
    // getting a socket
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Socket created" << endl;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    cout << "Connecting to the server ..." << endl;
    // connect the client socket to server socket
    while (connect(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect");
        cout << "Connection failed, retrying in 5 seconds" << endl;
        sleep(5);
    }
    cout << "Connection established...\n";
    int addrlen = sizeof(address);

    // server
    getsockname(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    char myIP[16];
    inet_ntop(AF_INET, &address.sin_addr, myIP, sizeof(myIP));
    int myPort = ntohs(address.sin_port);
    cout << "Local IP: " << myIP << endl;
    cout << "Local Port: " << myPort << endl;

    getpeername(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    char cIP[16];
    inet_ntop(AF_INET, &address.sin_addr, cIP, sizeof(cIP));
    int cPort = ntohs(address.sin_port);
    cout << "Server IP: " << cIP << endl;
    cout << "Server Port: " << cPort << endl;
    while (1)
    {
        string s;
        cout << "Enter text to capitalize: ";
        getline(cin, s);
        strcpy(buffer, s.c_str());
        send(sfd, buffer, s.size(), 0);
        memset(buffer, 0, 1024);
        valread = read(sfd, buffer, 1024);
        string msg(buffer);
        cout << msg << endl;
        memset(buffer, 0, 1024);
    }

    close(sfd);

    return 0;
}