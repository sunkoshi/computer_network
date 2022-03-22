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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <bits/stdc++.h>
using namespace std;

int main()
{
    int sfd;
    struct sockaddr_in saddr;
    char buf[1024];
    int opt = 1;
    int len = sizeof(saddr);
    int PORT;
    cout << "Enter the port for train...\n";
    cin >> PORT;
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    saddr.sin_port = htons(PORT);
    int i = 3, entered = 0;
    entered = connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr));
    cout << "Train is reaching the station...\n";
    cout << "Train is leaving the station...\n";
    return 0;
}