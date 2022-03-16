#include "my_fns.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>
#include <ace/Future.h>
#include <ace/Auto_Ptr.h>
#include <ace/config-lite.h>
bool isMMode = false;
class Server
{
private:
    ACE_INET_Addr serverAddr;
    ACE_INET_Addr clientAddr;
    ACE_SOCK_Acceptor acceptor;
    ACE_SOCK_Stream stream;

public:
    Server(int port) : serverAddr(port), acceptor(serverAddr)
    {
    }
    // accept
    int acceptConnection()
    {
        acceptor.get_local_addr(serverAddr);
        ACE_Time_Value timeout(10);
        acceptor.accept(stream, &clientAddr, &timeout);
        return stream.get_handle();
    }
};
void *DoServer(void *arg)
{
    int port = 8080;
    Server serverAddr(port);
    while (1)
    {

        int nsfd = serverAddr.acceptConnection();

        cout << "New client connected in S server..." << endl;
        int pid = GetPID(nsfd);
        cout << "PID: " << pid << endl;

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, ClientHandler, (void *)&nsfd);

        pids.push_back(pid);
        nsfds.push_back(nsfd);
        tids.push_back(client_thread);
    }
    return NULL;
}
int main()
{
    signal(SIGINT, CloseHandler);
    asfd = ConnectToServer("127.0.0.1", 3000);
    pthread_t id;
    pthread_create(&id, NULL, DoServer, NULL);

    do
    {
        cout << "Mode: " << (!isMMode ? "Working" : "Maintenance")
             << " Toggle? y/n";
        char ch;
        cin >> ch;
        isMMode = ch == 'y' ? !isMMode : isMMode;
        MaintainanceMode(isMMode);

    } while (true);
    return 0;
}