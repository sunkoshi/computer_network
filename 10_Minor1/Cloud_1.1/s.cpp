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
struct ServiceCount
{
    int serviceFd;
    int s1;
    int s2;
};
struct msg_st
{
    long int type;
    char msg_text[128];
};

vector<ServiceCount> cnt;
int mqId;
void UpdateCount(int fd, int s1, int s2)
{
    for (int i = 0; i < cnt.size(); i++)
    {
        if (cnt[i].serviceFd == fd)
        {
            cnt[i].s1 += s1;
            cnt[i].s2 += s2;
            return;
        }
    }
    ServiceCount sv;
    sv.serviceFd = fd;
    sv.s1 = s1;
    sv.s2 = s2;
    cnt.push_back(sv);
    cout << "---------------" << endl
         << "New Count" << endl;
    for (int i = 0; i < cnt.size(); i++)
    {
        cout << cnt[i].serviceFd << " "
             << "S1: " << cnt[i].s1 << " "
             << "S1: " << cnt[i].s2 << endl;
    }
    cout << endl;
}
void *ReadMsgQ(void *arg)
{
    cout << "Reading MesQ" << endl;
    for (int i = 0; i < 1; i++)
    {
        msg_st msg;
        msg.type = 1;
        int r = msgrcv(mqId, &msg, sizeof(msg), 1, 0);
        if (r >= 0)
        {
            string s = msg.msg_text;
            string pidS = "";
            bool isS1 = false;
            bool delim = false;
            for (char x : s)
            {
                if (x == '#')
                {
                    delim = true;
                }
                if (!delim)
                {
                    // pid
                    pidS.push_back(x);
                }
                else if (x != 's')
                {
                    if (x == '1')
                    {

                        {
                            isS1 = true;
                        }
                    }
                }
            }
            stringstream geek(pidS);
            int x = 0;
            geek >> x;
            UpdateCount(x, isS1 ? 1 : 0, isS1 ? 0 : 1);
            i--;
        }
    }
    return 0;
}
void HandleS1(int nsfd)
{
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
        transform(su.begin(), su.end(), su.begin(), ::toupper);
        su = "From capitalizer server: " + su;
        strcpy(buffer, su.c_str());
        send(nsfd, buffer, su.size(), 0);
        memset(buffer, 0, 1024);
    }
}

void HandleS2(int nsfd)
{
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
        transform(su.begin(), su.end(), su.begin(), ::tolower);
        su = "From lowerizer server: " + su;
        strcpy(buffer, su.c_str());
        send(nsfd, buffer, su.size(), 0);
        memset(buffer, 0, 1024);
    }
}

void HandleServices(int nsfd)
{
    int valread;
    char c;
    valread = read(nsfd, &c, 1);
    if (valread == 0)
    {
        cout << "Client disconnected" << endl;
        close(nsfd);
        return;
    }
    int rIndex = c;
    cout << "Service requested: " << rIndex << endl;
    if (rIndex == 1)
    {
        HandleS1(nsfd);
    }
    else
    {
        HandleS2(nsfd);
    }
}
void CloseMsgQ()
{
    msgctl(mqId, IPC_RMID, NULL);
}
void handleClose(int sig)
{
    CloseMsgQ();
    exit(0);
}
int main()
{
    signal(SIGINT, handleClose);
    CloseMsgQ();
    int key = ftok("mq", 65);
    mqId = msgget(key, 0666 | IPC_CREAT);

    pthread_t id;
    pthread_create(&id, NULL, ReadMsgQ, NULL);

    int PORT = 3000;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(sfd, (struct sockaddr *)&address, sizeof(address));
    cout << "Binded socket to address.." << endl;
    cout << "Socket listening on port " << PORT << endl;
    listen(sfd, SOMAXCONN);
    while (true)
    {
        struct sockaddr_in cliaddr;
        int addrlen = sizeof(cliaddr);
        int nsfd = accept(sfd, (struct sockaddr *)&cliaddr, (socklen_t *)&addrlen);
        if (nsfd < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        cout << "New client connected..." << endl;
        HandleServices(nsfd);
        cout << "------------------" << endl;
    }

    return 0;
}