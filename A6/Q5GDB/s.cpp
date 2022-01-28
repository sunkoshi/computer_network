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

string RecFile(int nsfd)
{
    string s = "";
    while (true)
    {
        char c;
        int a = read(nsfd, &c, 1);
        if (c == '\0' || a <= 0)
        {
            break;
        }
        s.push_back(c);
    }
    return s;
}
void WriteFile(string filename, string s)
{
    string clear = "echo  >" + filename;
    system(clear.c_str());
    int fd = open(filename.c_str(), O_RDWR, 0777);
    for (int i = 0; i < s.size(); i++)
    {
        char c = s[i];
        write(fd, &c, 1);
    }
    close(fd);
}
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

    // listen to the socket, with maximum buffer for clients
    if (listen(sfd, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    cout << "Socket listening on port " << PORT << endl;

    // accept the incoming connection and get nsfd
    if ((nsfd = accept(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    cout << "Client connected... " << endl;

    // rec prog
    string prog = RecFile(nsfd);
    WriteFile("./rp.cpp", prog);
    system("g++ ./rp.cpp -o rp");

    // rec inp
    string inp = RecFile(nsfd);
    WriteFile("./rinp.txt", inp);

    // rec out
    string out = RecFile(nsfd);

    // create a child process to handle the client

    pid_t pid = fork();
    if (pid > 0)
    {
        // parent
        wait(NULL);
        int poutFd = open("./gout.txt", O_RDWR, 0777);
        string rot = RecFile(poutFd);
        string msg;
        if (out == rot)
        {
            msg = "Test Case Passed";
        }
        else
        {
            msg = "Test Case Failed";
        }
        cout << msg << endl;
        send(nsfd, msg.c_str(), msg.size(), 0);
    }
    else
    {
        int itFd = open("./rinp.txt", O_RDWR, 0777);
        int poutFd = open("./gout.txt", O_RDWR, 0777);
        dup2(itFd, 0);
        dup2(poutFd, 1);
        execv("./rp", NULL);
    }

    // close the sockets
    close(nsfd);
    close(sfd);

    return 0;
}