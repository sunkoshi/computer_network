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
    struct sockaddr_in addr;
    if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
        memset(&addr, 0, sizeof(addr));
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    while (true)
    {
        string s = "";
        char buffer[MAXLINE];
        cout << "Enter text to capitalize: ";
        getline(cin, s);
        sendto(sfd, s.c_str(), s.size(),
               0, (const struct sockaddr *)&addr, sizeof(addr));
        int len = sizeof(addr);
        recvfrom(sfd, buffer, MAXLINE,
                 0, (struct sockaddr *)&addr,
                 (socklen_t *)&len);
        printf("Server : %s\n", buffer);
    }

    close(sfd);
    return 0;
}
