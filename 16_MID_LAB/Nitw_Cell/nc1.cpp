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
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
using namespace std;
#define MAXLINE 4096
int uSocket;
int udpsfd;
vector<sockaddr_in> pkgs;
vector<int> nsfds;
string ReadFD(int fd)
{
    string s = "";
    char ch;
    while (true)
    {
        int n = read(fd, &ch, 1);
        if (n <= 0 || ch == '\0')
        {
            break;
        }
        s.push_back(ch);
    }
    return s;
}
int WriteFD(int fd, string s)
{
    s.push_back('\0');
    return send(fd, s.c_str(), s.size(), 0);
}
void SendToUDP(int sfd, sockaddr_in addr, string s)
{
    s.push_back('\0');
    sendto(sfd, s.c_str(), s.size(),
           0, (const struct sockaddr *)&addr, sizeof(addr));
}
void SendFD(int socket, int fd)
{
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];
    /* storage space needed for an ancillary element with a payload of length is CMSG_SPACE(sizeof(length)) */
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
    int available_ancillary_element_buffer_space;

    /* at least one vector of one byte must be sent */
    message_buffer[0] = 'F';
    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = 1;

    /* initialize socket message */
    memset(&socket_message, 0, sizeof(struct msghdr));
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;

    /* provide space for the ancillary data */
    available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
    memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = available_ancillary_element_buffer_space;

    /* initialize a single ancillary data element for fd passing */
    control_message = CMSG_FIRSTHDR(&socket_message);
    control_message->cmsg_level = SOL_SOCKET;
    control_message->cmsg_type = SCM_RIGHTS;
    control_message->cmsg_len = CMSG_LEN(sizeof(int));
    *((int *)CMSG_DATA(control_message)) = fd;

    sendmsg(socket, &socket_message, 0);
}
int SocketServer()
{
    int PORT = 3000;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(sfd, (struct sockaddr *)&address, sizeof(address));
    cout << "Binded socket to address.." << endl;
    listen(sfd, SOMAXCONN);
    return sfd;
}
int UdsServer(string filename, bool doUnlink)
{
    struct sockaddr_un addr;
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (doUnlink)
        unlink(filename.c_str());
    memset(&addr, 0, sizeof(struct sockaddr_un));
    strncpy(addr.sun_path, filename.c_str(), sizeof(addr.sun_path) - 1);
    bind(sfd, (struct sockaddr *)&addr, sizeof(addr));
    listen(sfd, SOMAXCONN);
    return sfd;
}
void Poll(vector<int> arr)
{

    int size = arr.size();
    struct pollfd pfd[size];
    for (int i = 0; i < size; i++)
    {
        pfd[i].fd = arr[i];
        pfd[i].events = POLLIN;
        pfd[i].revents = 0;
    }
    cout << "Starting Poll" << endl;
    while (true)
    {
        int status = poll(pfd, size, -1);
        if (status < 0)
        {
            cout << "poll failed..." << endl;
            perror("poll");
            exit(1);
        }
        for (int i = 0; i < size; i++)
        {
            if (pfd[i].revents & POLLIN)
            {
                if (i == 0)
                {
                    cout << "New conn to socket server... " << endl;
                    // in sfd
                    // accepting the client in socket server
                    struct sockaddr_in address;
                    int addrlen = sizeof(address);
                    int nsfd = accept(pfd[0].fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

                    string config = ReadFD(nsfd);
                    char pkg = config[0];
                    int id = int(pkg) - 48;
                    cout << "Client is requesting for pkg: " << id << endl;
                    if (pkgs.size() + 1 >= id)
                    {
                        cout << "Following package is available here only...\n";
                        nsfds.push_back(nsfd);
                        char ip[16];
                        sockaddr_in clientAddr = pkgs[id - 1];
                        inet_ntop(AF_INET, &clientAddr.sin_addr, ip, sizeof(ip));
                        string s(ip);
                        cout << "Sending the addr of P that is " << s << endl;
                        WriteFD(nsfd, s);
                        for (int i = 0; i < pkgs.size(); i++)
                        {
                            SendToUDP(udpsfd, pkgs[i], s);
                        }
                    }
                    else
                    {
                        cout << "Following package is not available here...\n";
                        cout << "Sending customer to NC2...\n";
                        SendFD(uSocket, nsfd);
                    }
                }
                else
                {
                    // in usfd
                    cout << "NC2 server connected..." << endl;
                    uSocket = accept(pfd[1].fd, NULL, NULL);
                }
                pfd[i].revents = 0;
            }
        }
    }
}

void *ListenToPs(void *arg)
{
    int PORT = 5000;
    struct sockaddr_in addr;
    if ((udpsfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(udpsfd, (const struct sockaddr *)&addr,
             sizeof(addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    cout << "UDP listener for Ps is ready...\n";
    while (true)
    {
        struct sockaddr_in clientAddr;
        int len = sizeof(clientAddr);
        memset(&clientAddr, 0, sizeof(clientAddr));
        char buffer[MAXLINE];
        int n = recvfrom(udpsfd, (char *)buffer, MAXLINE,
                         0, (sockaddr *)&clientAddr,
                         (socklen_t *)&len);
        if (n <= 0)
        {
            continue;
        }

        cout << "Some P is sending: " << buffer << endl;
        // 1 free, 2 paid, 3 live
        char option = buffer[0];
        if (option == '0')
        {
            cout << "New P registering with tag " << buffer[1];
            cout << "\n";
            pkgs.push_back(clientAddr);
        }
    }
}

void *FreeSMS(void *arg)
{
    int sfd;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    if ((sfd = socket(PF_INET, SOCK_RAW, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8081);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    string s = "Free SMS Advertisement...";
    while (true)
    {
        sendto(sfd, s.c_str(), s.size(),
               0, (const struct sockaddr *)&addr, sizeof(addr));
        sleep(2);
    }
}

int main()
{
    // will connect to clients TCP
    int sfd = SocketServer();

    // for FD transfer
    int usfd = UdsServer("./uds.sock", true);

    // listen for Ps
    pthread_t id;
    pthread_create(&id, NULL, ListenToPs, NULL);

    // listen for Ps
    pthread_t id2;
    pthread_create(&id2, NULL, FreeSMS, NULL);

    vector<int> arr;
    arr.push_back(sfd);
    arr.push_back(usfd);
    Poll(arr);
    return 0;
}