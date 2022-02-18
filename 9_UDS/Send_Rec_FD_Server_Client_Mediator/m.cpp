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
int uSocket;
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
                    // now send fd to usfd
                    SendFD(uSocket, nsfd);
                    cout << "Sent NSFD: " << nsfd << endl;
                }
                else
                {
                    // in usfd
                    cout << "New conn to Uds server..." << endl;
                    uSocket = accept(pfd[1].fd, NULL, NULL);
                }
                pfd[i].revents = 0;
            }
        }
    }
}

int main()
{
    int sfd = SocketServer();
    int usfd = UdsServer("./uds.sock", true);
    vector<int> arr;
    arr.push_back(sfd);
    arr.push_back(usfd);
    Poll(arr);
    return 0;
}