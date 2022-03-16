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
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Dgram.h>
#include <ace/OS_NS_unistd.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>
#include <ace/Task.h>
#include <ace/UNIX_Addr.h>
#include <ace/Future.h>
#include <ace/Auto_Ptr.h>
#include <ace/config-lite.h>
#define MSG_CMSG_CLOEXEC 0x40000000
using namespace std;
class Server
{
private:
    ACE_UNIX_Addr serverAddr;
    ACE_UNIX_Addr clientAddr;
    ACE_SOCK_Acceptor acceptor;
    ACE_SOCK_Stream stream;

public:
    Server(char *filename) : serverAddr(filename), acceptor(serverAddr)
    {
    }
    int getFD()
    {
        int socket = stream.get_handle();
        int sent_fd, available_ancillary_element_buffer_space;
        struct msghdr socket_message;
        struct iovec io_vector[1];
        struct cmsghdr *control_message = NULL;
        char message_buffer[1];
        char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
        memset(&socket_message, 0, sizeof(struct msghdr));
        memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));
        io_vector[0].iov_base = message_buffer;
        io_vector[0].iov_len = 1;
        socket_message.msg_iov = io_vector;
        socket_message.msg_iovlen = 1;
        socket_message.msg_control = ancillary_element_buffer;
        socket_message.msg_controllen = CMSG_SPACE(sizeof(int));
        if (recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
            return -1;
        if (message_buffer[0] != 'F')
        {
            return -1;
        }
        if ((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
        {
            return -1;
        }
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
    int acceptConnection()
    {
        acceptor.get_local_addr(serverAddr);
        ACE_Time_Value timeout(10);
        acceptor.accept(stream, &clientAddr, &timeout);
    }
};

int main()
{
    int port = 8080;
    Server server("ace.sock");
    server.acceptConnection();
    cout << "Waiting for FD... " << endl;
    int rFd = server.getFD();
    cout << "Reading from recv. fd... \n";
    char ch;
    while (true)
    {
        int n = read(rFd, &ch, 1);
        if (n <= 0)
            return 0;
        cout << ch;
    }
}
