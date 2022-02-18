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
using namespace std;
void *ClientHandler(void *arg);
int sfd;
struct thread_data
{
    int nsfd;
};
void Listen()
{
    // listen to the socket, with maximum buffer for clients
    if (listen(sfd, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        int nsfd;
        // accept the incoming connection and get nsfd
        if ((nsfd = accept(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        cout << "New client connected... " << endl;
        struct thread_data data;
        data.nsfd = nsfd;

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, ClientHandler, (void *)&data);
    }
}
void CloseHandler(int count)
{
    cout << "Closing \n";
    close(sfd);
}

void *ClientHandler(void *arg)
{
    struct thread_data *data;
    data = (struct thread_data *)arg;
    int nsfd = data->nsfd;
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

int main()
{
    signal(SIGINT, CloseHandler);
    int PORT = 8080;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

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
    cout << "Socket listening on port " << PORT << endl;

    // blocking call
    Listen();

    // close the sockets

    return 0;
}