
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
#include <poll.h>
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
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/tcp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <bits/stdc++.h>
using namespace std;
#define UDS_SERVER_PATH "./uds.soc"

struct ServerData
{
    string ip;
    int port;
};

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

int UdsServer(string sun_file)
{
    int usfd;
    unlink(sun_file.c_str());
    struct sockaddr_un server;
    usfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (usfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, sun_file.c_str());
    unlink(server.sun_path);
    int len = strlen(server.sun_path) + sizeof(server.sun_family);
    if (bind(usfd, (struct sockaddr *)&server, len) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    cout << "Initiated UDS server" << endl;
    return usfd;
}

int SockServer(int PORT)
{
    int sfd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
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
    return sfd;
}

// to connect to a tcp server
int ConnectToTcpServer(string ip, int PORT)
{
    struct sockaddr_in address;
    int csfd;
    if ((csfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Socket created..." << endl;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    cout << "Connecting to the server ..." << endl;
    // connect the client socket to server socket
    if (connect(csfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect");
        cout << "Connection failed," << ip << " " << PORT << endl;
    }
    printf("Connection established with ip: %s, port:%d\n", ip.c_str(), PORT);
    return csfd;
}

// connect to uds server
int ConnectToUdsServer(string sun_file)
{
    struct sockaddr_un address;
    int usfd;
    if ((usfd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Socket created..." << endl;

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, sun_file.c_str());

    cout << "Connecting to the server ..." << endl;
    // connect the client socket to server socket
    if (connect(usfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect");
        cout << "Connection failed," << sun_file << endl;
        exit(1);
    }
    printf("Connection established..\n");
    return usfd;
}

void sendFD(int socket, int fd)
{
    struct msghdr msg = {0};
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE(sizeof(int))], dup[256];
    memset(buf, '\0', sizeof(buf));
    struct iovec io = {.iov_base = &dup, .iov_len = sizeof(dup)};

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));

    memcpy((int *)CMSG_DATA(cmsg), &fd, sizeof(int));

    if (sendmsg(socket, &msg, 0) < 0)
        perror("Failed to send message");
}
int getFD(int socket)
{

    int *fd = new int;
    struct msghdr msg = {0};
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE(sizeof(int))], dup[256];
    memset(buf, '\0', sizeof(buf));
    struct iovec io = {.iov_base = &dup, .iov_len = sizeof(dup)};

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    if (recvmsg(socket, &msg, 0) < 0)
        perror("Failed to receive message");

    cmsg = CMSG_FIRSTHDR(&msg);

    memcpy(fd, (int *)CMSG_DATA(cmsg), sizeof(int));

    return *fd;
}

int RawSockServer(int protocol)
{
    int rsfd;
    if ((rsfd = socket(AF_INET, SOCK_RAW, protocol)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return rsfd;
}