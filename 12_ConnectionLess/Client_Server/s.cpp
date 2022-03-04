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
#define PORT 8080
#define MAXLINE 1024
#define MSG_CONFIRM 0x800

int main()
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

    while (true)
    {
        char buffer[MAXLINE];
        // this will not only rec message but also the client addr
        int n = recvfrom(sfd, (char *)buffer, MAXLINE,
                         0, (sockaddr *)&clientAddr,
                         (socklen_t *)&len);
        if (n <= 0)
        {
            continue;
        }
        string s(buffer);
        memset(buffer, 0, strlen(buffer));
        cout << "Client said: " << s << endl;
        transform(s.begin(), s.end(), s.begin(), ::toupper);
        cout << "Sending response as: " << s << endl;
        sendto(sfd, s.c_str(), s.size(),
               0, (const struct sockaddr *)&clientAddr,
               sizeof(clientAddr));
    }
    return 0;
}
