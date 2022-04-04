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
#define MSG_CMSG_CLOEXEC 0x40000000
vector<int> nsfds;
int GetFD(int socket)
{
    int sent_fd, available_ancillary_element_buffer_space;
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

    /* start clean */
    memset(&socket_message, 0, sizeof(struct msghdr));
    memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));

    /* setup a place to fill in message contents */
    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = 1;
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;

    /* provide space for the ancillary data */
    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

    if (recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
        return -1;

    if (message_buffer[0] != 'F')
    {
        /* this did not originate from the above function */
        return -1;
    }

    if ((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
    {
        /* we did not provide enough space for the ancillary element array */
        return -1;
    }

    /* iterate ancillary elements */
    for (control_message = CMSG_FIRSTHDR(&socket_message);
         control_message != NULL;
         control_message = CMSG_NXTHDR(&socket_message, control_message))
    {
        if ((control_message->cmsg_level == SOL_SOCKET) &&
            (control_message->cmsg_type == SCM_RIGHTS))
        {
            sent_fd = *((int *)CMSG_DATA(control_message));
            return sent_fd;
        }
    }

    return -1;
}

void *HandleClient(void *arg)
{
    int sfd = *(int *)arg;
    while (true)
    {
        int nsfd = GetFD(sfd);
        nsfds.push_back(nsfd);
        cout << "Got NSFD: " << nsfd << endl;
        int valread;
        char buffer[1024] = {0};
        while (true)
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
            su = "From server: " + su;
            strcpy(buffer, su.c_str());
            send(nsfd, buffer, su.size(), 0);
            memset(buffer, 0, 1024);
        }
    }
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

int main()
{
    string filename = "./uds.sock";
    struct sockaddr_un addr;
    int usfd = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(struct sockaddr_un));
    strncpy(addr.sun_path, filename.c_str(), sizeof(addr.sun_path) - 1);
    bind(usfd, (struct sockaddr *)&addr, sizeof(addr));
    if (connect(usfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
    {
        cout << "Error connecting to uds server..." << endl;
        exit(1);
    }
    cout << "Connected to NC1 server..." << endl;
    return 0;

    
}