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
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Dgram.h>
#include <ace/OS_NS_unistd.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>
#include <ace/Task.h>
#include <ace/Future.h>
#include <ace/Auto_Ptr.h>
#include <ace/UNIX_Addr.h>
#include <ace/config-lite.h>
#include <bits/stdc++.h>
using namespace std;
class Client
{
private:
    ACE_SOCK_Stream stream;
    ACE_UNIX_Addr serverAddr;
    ACE_SOCK_Connector connector;

public:
    Client(char *filename) : serverAddr(filename)
    {
    }
    int connectToServer()
    {
        connector.connect(stream, serverAddr);
        return stream.get_handle();
    }

    void sendFD(int fd)
    {
        int socket = stream.get_handle();
        struct msghdr socket_message;
        struct iovec io_vector[1];
        struct cmsghdr *control_message = NULL;
        char message_buffer[1];
        char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
        int available_ancillary_element_buffer_space;
        message_buffer[0] = 'F';
        io_vector[0].iov_base = message_buffer;
        io_vector[0].iov_len = 1;
        memset(&socket_message, 0, sizeof(struct msghdr));
        socket_message.msg_iov = io_vector;
        socket_message.msg_iovlen = 1;
        available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
        memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
        socket_message.msg_control = ancillary_element_buffer;
        socket_message.msg_controllen = available_ancillary_element_buffer_space;
        control_message = CMSG_FIRSTHDR(&socket_message);
        control_message->cmsg_level = SOL_SOCKET;
        control_message->cmsg_type = SCM_RIGHTS;
        control_message->cmsg_len = CMSG_LEN(sizeof(int));
        *((int *)CMSG_DATA(control_message)) = fd;
        sendmsg(socket, &socket_message, 0);
    }
    void close()
    {
        stream.close();
    }
};
int main(int argc, char *argv[])
{
    int fileFd = open("a.txt", 0666);
    cout << "Sending File FD..." << endl;
    int port = 8080;
    Client client("ace.sock");
    client.connectToServer();
    client.sendFD(fileFd);
    client.close();
}