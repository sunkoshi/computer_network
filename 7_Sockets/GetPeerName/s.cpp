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
    int PORT = 8080;
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
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // binding the address to sfd
    if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "Binded socket to address.." << endl;
    getsockname(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    char myIP[16];
    inet_ntop(AF_INET, &address.sin_addr, myIP, sizeof(myIP));
    int myPort = ntohs(address.sin_port);
    cout << "Local IP: " << myIP << endl;
    cout << "Local Port: " << myPort << endl;

    // listen to the socket, with maximum buffer for clients
    if (listen(sfd, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    cout << "Socket listening on port " << PORT << endl;

    while (true)
    {
        // accept the incoming connection and get nsfd
        if ((nsfd = accept(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        cout << "Client connected... " << endl;
        getpeername(nsfd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        char cIP[16];
        inet_ntop(AF_INET, &address.sin_addr, cIP, sizeof(cIP));
        int cPort = ntohs(address.sin_port);
        cout << "Client IP: " << cIP << endl;
        cout << "Client Port: " << cPort << endl;

        // read from client
        valread = read(nsfd, buffer, 1024);
        string su(buffer);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            close(nsfd);
            continue;
        }

        cout << "Client sent: " << buffer << endl;
        memset(buffer, 0, 1024);
        transform(su.begin(), su.end(), su.begin(), ::toupper);
        su = "From capitalizer server: " + su;
        strcpy(buffer, su.c_str());
        send(nsfd, buffer, su.size(), 0);
        memset(buffer, 0, 1024);
        close(nsfd);
    }

    // close the sockets
    close(nsfd);
    close(sfd);
    return 0;
}