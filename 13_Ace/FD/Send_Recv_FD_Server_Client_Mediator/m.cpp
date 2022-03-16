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
class Server
{
private:
    ACE_INET_Addr serverAddr;
    ACE_INET_Addr clientAddr;
    ACE_SOCK_Acceptor acceptor;
    ACE_SOCK_Stream clientConn;

public:
    Server(int port) : serverAddr(port), acceptor(serverAddr)
    {
    }
    int handleConnection()
    {
    }
    int acceptConnection()
    {
        acceptor.get_local_addr(serverAddr);
        while (1)
        {
            ACE_Time_Value timeout(10);
            acceptor.accept(clientConn, &clientAddr, &timeout);
            handleConnection();
        }
    }
};

int main()
{
    int port = 8080;
    Server serverAddr(port);
    serverAddr.acceptConnection();
}