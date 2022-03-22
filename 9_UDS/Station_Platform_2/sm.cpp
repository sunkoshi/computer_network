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
#include <poll.h>
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
int GetFreePlatform(int avail[], int n)
{
    int ret = 0;
    // identify first index with avail[i] = 1;
    for (int i = 0; i < n; i++)
    {
        if (avail[i] == 1)
        {
            return i;
        }
    }

    return -1;
}

void FanoutMsg(int psfds[], int no_pforms)
{
    // broadcast the message to all the connected platforms
    for (int i = 0; i < no_pforms; i++)
    {
        string message = "New train arriving...\n";
        send(psfds[i], message.c_str(), sizeof(message.c_str()), 0);
    }
}

int main()
{
    int no_pforms = 3;
    int tsfd[3] = {0}, psfd, psfds[no_pforms];
    int available_platform[3] = {0};
    struct sockaddr_in taddr[3];
    struct sockaddr_un paddr_un;
    char buf[1024];
    int opt = 1;
    int PORT = 5000;
    string server_socket_path = "./train.sock";
    memset(&paddr_un, 0, sizeof(paddr_un));
    psfd = socket(AF_UNIX, SOCK_STREAM, 0);
    cout << "Waiting for platforms...\n";
    paddr_un.sun_family = AF_UNIX;
    strcpy(paddr_un.sun_path, server_socket_path.c_str());
    int len = sizeof(paddr_un);
    unlink(paddr_un.sun_path);
    bind(psfd, (struct sockaddr *)&paddr_un, len);
    listen(psfd, 3);
    for (int i = 0; i < 3; i++)
    {
        psfds[i] = accept(psfd, (struct sockaddr *)&paddr_un, (socklen_t *)&len);
        cout << "New platform added...\n";
        available_platform[i] = 1;
    }

    cout << "Waiting for trains...\n";
    for (int i = 0; i < no_pforms; i++)
    {
        tsfd[i] = socket(AF_INET, SOCK_STREAM, 0);

        taddr[i].sin_family = AF_INET;
        taddr[i].sin_addr.s_addr = INADDR_ANY;
        taddr[i].sin_port = htons(PORT + i + 1);

        bind(tsfd[i], (struct sockaddr *)&taddr[i], sizeof(taddr[i]));
        listen(tsfd[i], 3);
    }
    struct pollfd pfds[no_pforms];
    for (int i = 0; i < no_pforms; i++)
    {
        pfds[i].fd = tsfd[i];
        pfds[i].events = POLL_IN;
        pfds[i].revents = 0;
    }

    len = sizeof(taddr[0]);
    while (1)
    {
        int ready = poll(pfds, no_pforms, 5000);
        int nsfd;
        int idx;
        if (ready > 0 && (idx = GetFreePlatform(available_platform, no_pforms)) != -1)
        {
            for (int i = 0; i < 3 && ready > 0; i++)
            {
                if (((idx = GetFreePlatform(available_platform, no_pforms) != -1) && (POLL_IN & pfds[i].revents)))
                {
                    available_platform[idx] = 0;
                    if (nsfd = (accept(tsfd[i], (struct sockaddr *)&taddr[0], (socklen_t *)&len)) == -1)
                    {
                        perror("accept");
                        exit(1);
                    }
                    cout << "Train entering station...\n";

                    SendFD(psfds[i], nsfd);
                    close(nsfd);
                    RecFD(psfds[i]);

                    cout << "Train leaving station...\n";
                    available_platform[idx] = 1;
                }
            }
        }
    }
}