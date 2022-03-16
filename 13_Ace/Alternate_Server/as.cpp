#include "my_fns.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>
#include <ace/Future.h>
#include <ace/Auto_Ptr.h>
#include <ace/config-lite.h>
void Poll(vector<int> fds)
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
                    pollfds[i].revents = 0;

                    if (i == 0)
                    {
                        // on port 3000
                        // reading messages
                        int nsfd = pollfds[0].fd;
                        string s = ReadFD(nsfd);
                        if (s.size() == 0)
                            continue;

                        if (s == "1")
                        {
                            cout << "S going in maintainance mode...\n";
                            // requesting for usage of alternate server
                            // rec all the valid pids
                            while (true)
                            {
                                string s = ReadFD(nsfd);
                                if (s.size() == 0 || s == "end")
                                    break;
                                int pid = atoi(s.c_str());
                                cout << "Rec PID: " << pid << endl;
                                pids.push_back(pid);
                            }
                            int n = pids.size();
                            printf("Rec %d pids to service...\n", n);
                        }
                        else
                        {
                            cout << "S is restoring its status to working mode...\n";
                            // telling to stop providing the service
                            for (int i = 0; i < nsfds.size(); i++)
                            {
                                SendServerChangeMessage("127.0.0.1", 8080, nsfds[i]);
                                pthread_cancel(tids[i]);
                            }
                            pids.clear();
                        }
                    }
                    else
                    {
                        // on port 8081

                        struct sockaddr_in cliaddr;
                        int addrlen = sizeof(cliaddr);
                        int nsfd = accept(pollfds[i].fd, NULL, NULL);
                        if (nsfd < 0)
                        {
                            perror("accept");
                            exit(EXIT_FAILURE);
                        }

                        cout << "New client connected in AS server..." << endl;
                        int pid = GetPID(nsfd);
                        cout << "Client says its PID is: " << pid << endl;

                        // alternate server
                        // allow only certain pids
                        if (IfPIDExists(pid))
                        {
                            printf("Allowing the client with pid: %d....\n", pid);
                            pthread_t client_thread;
                            pthread_create(&client_thread, NULL, ClientHandler, (void *)&nsfd);
                            nsfds.push_back(nsfd);
                            tids.push_back(client_thread);
                        }
                        else
                        {
                            cout << "Removing the client....\n";
                            close(nsfd);
                        }
                    }
                }
            }
        }
    }
}
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
    int getFd()
    {
        return acceptor.get_handle();
    }
};
int main()
{
    signal(SIGINT, CloseHandler);
    cout << "Waiting for S to connect...\n";
    Server ss(3000);
    int nsfd = ss.acceptConnection();
    cout << "S is connected... \n";

    Server s(8081);
    cout << "S connected...\n";
    int sfd = s.getFd();
    vector<int> fds;
    fds.push_back(nsfd);
    fds.push_back(sfd);
    Poll(fds);

    return 0;
}