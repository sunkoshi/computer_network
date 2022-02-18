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
#define MSG_CMSG_CLOEXEC 0x40000000
using namespace std;
struct thread_data
{
    int sfd;
};
struct Platform
{
    bool isFree;
    int tsfd;
    int psfd;
};
vector<Platform> platArr;
vector<int> trainArr;
Platform GetFreePlatform()
{
    for (int i = 0; i < platArr.size(); i++)
    {
        if (platArr[i].isFree)
        {
            return platArr[i];
        }
    }
    Platform p;
    p.isFree = false;
    return p;
}
void AssignPlatformToTrain(int psfd, int tsfd)
{
    for (int i = 0; i < platArr.size(); i++)
    {
        if (platArr[i].psfd == psfd)
        {
            platArr[i].tsfd = tsfd;
            platArr[i].isFree = false;
            break;
        }
    }
}
void FreePlatform(int psfd)
{
    for (int i = 0; i < platArr.size(); i++)
    {
        if (platArr[i].psfd == psfd)
        {
            platArr[i].tsfd = -1;
            platArr[i].isFree = true;
            break;
        }
    }
}
string ReadFDToString(int fd)
{
    string s = "";
    while (true)
    {
        char c;
        int a = read(fd, &c, 1);
        if (a == 0)
        {
            break;
        }
        s.push_back(c);
    }
    return s;
}
void WriteStringToFD(string text, int fd)
{
    send(fd, text.c_str(), text.size(), 0);
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

int RecFD(int socket)
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
int CreateSocketServer(int PORT)
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // socket file descriptor
    // getting a socket
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
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
    bind(sfd, (struct sockaddr *)&address, sizeof(address));

    cout << "Binded socket to address.." << endl;

    return sfd;
}

int CreateUDSServer(string filename, bool doUnlink)
{
    struct sockaddr_un addr;
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (doUnlink)
        unlink(filename.c_str());
    memset(&addr, 0, sizeof(struct sockaddr_un));
    strncpy(addr.sun_path, filename.c_str(), sizeof(addr.sun_path) - 1);
    bind(sfd, (struct sockaddr *)&addr, sizeof(addr));

    return sfd;
}

// ************************ Platform Handler ***********************
void *PlatformHandler(void *arg)
{
    struct thread_data *data;
    data = (struct thread_data *)arg;
    int psfd = data->sfd;
    Platform p;
    p.isFree = true;
    p.tsfd = -1;
    p.psfd = psfd;
    platArr.push_back(p);
    cout << "Total platforms available = " << platArr.size() << endl;
    while (1)
    {
        string s = ReadFDToString(psfd);
        cout << s << endl;
        if (s == "left")
        {
            cout << "Train left the platform...";
            FreePlatform(psfd);
        }
        if (s.size() == 0)
        {
            cout << "Platform disconnected..." << endl;
            break;
        }
        cout << s << endl;
    }
    return (NULL);
}

// ************************ Train Handler ***********************
void *TrainHandler(void *arg)
{
    struct thread_data *data;
    data = (struct thread_data *)arg;
    int tsfd = data->sfd;
    int psfd;
    cout << "Handling train with fd = " << tsfd << endl;
    // cout << "Finding free platform for train with fd= " << tsfd << endl;
    // while (true)
    // {
    //     Platform p = GetFreePlatform();
    //     if (p.isFree)
    //     {
    //         psfd = p.psfd;
    //         break;
    //     }
    // }

    // cout << "Train with fd= " << tsfd << " assigned to platform with fd= " << psfd << endl;

    // assigning train to platfrom
    // AssignPlatformToTrain(psfd, tsfd);

    // sending train's fd to platform fd
    // SendFD(psfd, tsfd);
    while (true)
    {
        char buffer[1024] = {0};
        int valread = read(psfd, buffer, 1024);
        string su(buffer);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            break;
            close(psfd);
        }
        cout << "Client sent: " << buffer << endl;
        memset(buffer, 0, 1024);
        transform(su.begin(), su.end(), su.begin(), ::toupper);
        su = "From lowerizer server: " + su;
        strcpy(buffer, su.c_str());
        send(psfd, buffer, su.size(), 0);
        memset(buffer, 0, 1024);
    }
    return (NULL);
}

void PollListenToServers(vector<int> fds)
{
    int pSize = fds.size();
    struct pollfd pollfds[pSize];
    for (int i = 0; i < pSize; i++)
    {
        struct pollfd pollfd;
        pollfd.fd = fds[i];
        pollfd.events = POLLIN | POLLPRI;
        pollfd.revents = 0;
        pollfds[i] = pollfd;
    }
    while (true)
    {
        int pollResult = poll(pollfds, pSize, -1);
        if (pollResult > 0)
        {
            for (int i = 0; i < pSize; i++)
            {
                if (pollfds[i].revents & POLLIN)
                {
                    struct sockaddr_in cliaddr;
                    int addrlen = sizeof(cliaddr);
                    int nsfd = accept(pollfds[i].fd, NULL, NULL);
                    if (nsfd < 0)
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    pollfds[i].revents = 0;

                    struct thread_data data;
                    data.sfd = nsfd;

                    if (i == 0)
                    {
                        pthread_t client_thread;
                        // uds item
                        cout << "New platform connected in server..." << endl;
                        pthread_create(&client_thread, NULL, PlatformHandler, (void *)&data);
                    }

                    else
                    {
                        pthread_t client_thread;
                        // socket item
                        cout << "New train connected in server..." << endl;
                        pthread_create(&client_thread, NULL, TrainHandler, (void *)&data);
                    }
                }
            }
        }
    }
}