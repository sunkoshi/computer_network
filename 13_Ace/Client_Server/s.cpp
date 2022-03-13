#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <ace/OS.h>
#include <ace/SOCK_Dgram.h>
#include <ace/OS_NS_unistd.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>
#include <ace/Task.h>
#include <ace/Future.h>
#include <ace/Auto_Ptr.h>
#include <ace/config-lite.h>
#define BUF_SIZE 1024
#define NO_ITERATIONS 1
class Server
{
private:
    char *buffer;
    ACE_INET_Addr serverAddr;
    ACE_INET_Addr clientAddr;
    ACE_SOCK_Acceptor peer_acceptor_;
    ACE_SOCK_Stream clientConn;

public:
    Server(int port) : serverAddr(port), peer_acceptor_(serverAddr)
    {
        buffer = new char[BUF_SIZE];
    }
    int handleConnection()
    {
        // Read data from client
        clientConn.recv_n(buffer, BUF_SIZE, 0);
        printf("Client Told: %s \n", buffer);

        // close the client connection
        clientConn.close();
        return 0;
    }
    // accept
    int acceptConnections()
    {
        peer_acceptor_.get_local_addr(serverAddr);
        printf("Starting serverAddr at port %d\n", serverAddr.get_port_number());

        // continuously look for accepting connections
        while (1)
        {
            // timeout of 10s
            ACE_Time_Value timeout(10);
            peer_acceptor_.accept(clientConn, &clientAddr, &timeout);

            printf("Connection established with remote %s:%d\n",
                   clientAddr.get_host_name(), clientAddr.get_port_number());
            // Handle the connection
            handleConnection();
        }
    }
};

int main()
{
    int port = 8080;
    Server serverAddr(port);
    serverAddr.acceptConnections();
}