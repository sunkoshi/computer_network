#include "my_fns.h"
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
#include <ace/config-lite.h>
#include <bits/stdc++.h>
using namespace std;
class Client
{
private:
    ACE_SOCK_Stream stream;
    ACE_INET_Addr serverAddr;
    ACE_SOCK_Connector connector;

public:
    Client(string hostname, int port) : serverAddr(port, hostname.c_str())
    {
    }
    int connectToServer()
    {
        connector.connect(stream, serverAddr);
        return stream.get_handle();
    }

    void send(string s)
    {
        stream.send_n(s.c_str(), ACE_OS::strlen(s.c_str()) + 1, 0);
    }

    string recv()
    {
        char *ch;
        stream.recv_n(ch, ACE_OS::strlen(ch) + 1, 0);
        return string(ch);
    }
    void close()
    {
        stream.close();
    }
};
int main(int argc, char *argv[])
{
    string pidString = to_string(getpid());
    int PORT = 8080;
    string ip = "127.0.0.1";
    while (true)
    {
        Client client(ip, PORT);
        client.connectToServer();
        client.send(pidString);
        while (true)
        {
            string s = client.recv();
            cout << "Server said: " << s << endl;
            if (s == "")
            {
                break;
            }
            if (s[0] == '#')
            {
                cout << "Got Server Change Signal...\n";
                ServerData data = IntepretServerChangeMessage(s);
                PORT = data.port;
                ip = data.ip;
                printf("IP: %s, PORT: %d\n", ip.c_str(), PORT);
                client.close();
                break;
            }
        }
    }
}
