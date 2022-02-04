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
void *ClientHandler(void *arg);

struct thread_data
{
    int nsfd;
};

void *HandleS1(void *arg)
{
    struct thread_data *data;
    data = (struct thread_data *)arg;
    int nsfd = data->nsfd;
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
        su = "Server: ok";
        strcpy(buffer, su.c_str());
        send(nsfd, buffer, su.size(), 0);
        memset(buffer, 0, 1024);
    }
    return 0;
}

void *HandleS2(void *arg)
{
    struct thread_data *data;
    data = (struct thread_data *)arg;
    int nsfd = data->nsfd;
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
        transform(su.begin(), su.end(), su.begin(), ::toupper);
        su = "From capitalizer server: " + su;
        strcpy(buffer, su.c_str());
        send(nsfd, buffer, su.size(), 0);
        memset(buffer, 0, 1024);
    }
    return 0;
}

void *HandleS3(void *arg)
{
    struct thread_data *data;
    data = (struct thread_data *)arg;
    int nsfd = data->nsfd;
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
        transform(su.begin(), su.end(), su.begin(), ::tolower);
        su = "From lowerizer server: " + su;
        strcpy(buffer, su.c_str());
        send(nsfd, buffer, su.size(), 0);
        memset(buffer, 0, 1024);
    }
    return 0;
}
int main()
{
    int pSize = 3;
    struct pollfd pollfds[pSize];
    for (int i = 0; i < 3; i++)
    {
        int PORT = 8080 + i;
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        int sfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sfd == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);
        if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        cout << "Binded socket to address.." << endl;
        cout << "Socket listening on port " << PORT << endl;

        pollfds[i].fd = sfd;
        pollfds[i].events = POLLIN | POLLPRI;
        pollfds[i].revents = 0;

        if (listen(sfd, SOMAXCONN) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
    }

    while (true)
    {
        int pollResult = poll(pollfds, pSize, -1);
        if (pollResult > 0)
        {
            for (int i = 0; i < pSize; i++)
            {
                if (pollfds[i].revents & POLLIN)
                {
                    struct sockaddr_in cliaddr;
                    int addrlen = sizeof(cliaddr);
                    int nsfd = accept(pollfds[i].fd, (struct sockaddr *)&cliaddr, (socklen_t *)&addrlen);
                    if (nsfd < 0)
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    pollfds[i].revents = 0;

                    cout << "New client connected in server " << i << endl;
                    struct thread_data data;
                    data.nsfd = nsfd;

                    pthread_t client_thread;

                    if (i == 0)
                    {
                        pthread_create(&client_thread, NULL, HandleS1, (void *)&data);
                    }
                    else if (i == 1)
                    {
                        pthread_create(&client_thread, NULL, HandleS2, (void *)&data);
                    }
                    else
                    {
                        pthread_create(&client_thread, NULL, HandleS3, (void *)&data);
                    }
                }
            }
        }
    }

    return 0;
}