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

int main(int argc, char *argv[])
{

    int PORT = 3000;
    struct sockaddr_in address;
    char buffer[1024] = {0};
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    cout << "Socket created..." << endl;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
    cout << "Connecting to the server ..." << endl;
    connect(sfd, (struct sockaddr *)&address, sizeof(address));
    cout << "Connection established...\n";
    while (true)
    {
        string s;
        cout << "Enter text: ";
        cin >> s;
        strcpy(buffer, s.c_str());
        send(sfd, buffer, s.size(), 0);
        memset(buffer, 0, 1024);
        int valread = read(sfd, buffer, 1024);
        string msg(buffer);
        cout << msg << endl;
        memset(buffer, 0, 1024);
    }
    close(sfd);
    return 0;
}