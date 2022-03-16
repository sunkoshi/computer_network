#include "my_fns.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Method_Request.h>
#include <ace/Future.h>
#include <ace/Auto_Ptr.h>
#include <ace/config-lite.h>
using namespace std;
class Client
{
private:
    ACE_SOCK_Stream client_stream_;
    ACE_INET_Addr serverAddr;
    ACE_SOCK_Connector connector;
    char *buffer;

public:
    Client(char *hostname, int port) : serverAddr(port, hostname)
    {
        buffer = "Hello Server";
    }
    int connectToServer()
    {
        connector.connect(client_stream_, serverAddr);
        return client_stream_.get_handle();
    }

    void sendToServer()
    {
        client_stream_.send_n(buffer, ACE_OS::strlen(buffer) + 1, 0);
    }

    // closing the connection
    void close()
    {
        client_stream_.close();
    }
};
int main(int argc, char *argv[])
{
    int port = 8080;
    Client client("127.0.0.1", port);
    client.connectToServer();
    client.close();
}