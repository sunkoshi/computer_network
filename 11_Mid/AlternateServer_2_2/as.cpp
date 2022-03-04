#include "my_fns.h"
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
int main()
{
    signal(SIGINT, CloseHandler);
    int ssfd = ServerInit(3000);
    sfd = ServerInit(8081);
    listen(ssfd, SOMAXCONN);
    int nsfd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    cout << "Waiting for S to connect...\n";
    if ((nsfd = accept(ssfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    cout << "S connected...\n";
    listen(sfd, SOMAXCONN);
    vector<int> fds;
    fds.push_back(nsfd);
    fds.push_back(sfd);
    Poll(fds);

    return 0;
}