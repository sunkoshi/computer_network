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
using namespace std;
void *ClientHandler(void *arg);
int sfd;
vector<int> sfds;
struct thread_data
{
    int nsfd;
};

void Listen()
{
    // listen to the socket, with maximum buffer for clients
    if (listen(sfd, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        int nsfd;
        // accept the incoming connection and get nsfd
        if ((nsfd = accept(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        cout << "New client connected... " << endl;
        struct thread_data data;
        data.nsfd = nsfd;

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, ClientHandler, (void *)&data);
    }
}
vector<int> ConnectToServers(vector<int> ports)
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    vector<int> sfds;
    for (int x = 0; x < ports.size(); x++)
    {
        int port = ports[x];
        if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        cout << "Socket created" << endl;

        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
        {
            perror("inet_pton");
            exit(EXIT_FAILURE);
        }

        cout << "Connecting to the server ..." << endl;
        // connect the client socket to server socket
        while (connect(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            perror("connect");
            cout << "Connection failed, retrying in 5 seconds" << endl;
            sleep(5);
        }
        cout << "Connection established...\n";
        sfds.push_back(sfd);
    }
    return sfds;
}

void *ClientHandler(void *arg)
{
    struct thread_data *data;
    data = (struct thread_data *)arg;
    int nsfd = data->nsfd;
    int valread;
    char buffer[1024] = {0};
    while (1)
    {

        // read from client
        valread = read(nsfd, buffer, 1024);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            break;
            close(nsfd);
        }
        int rIndex = buffer[0];
        cout << "Client wants to connect to server " << rIndex << endl;
        cout << "Client sent: " << buffer << endl;
        string s = "";
        for (int i = 1; i < strlen(buffer); i++)
        {
            s.push_back(buffer[i]);
        }

        // reset buffer
        memset(buffer, 0, 1024);
        strcpy(buffer, s.c_str());

        // send to server
        send(sfds[rIndex], buffer, s.size(), 0);
        memset(buffer, 0, 1024);

        // wait for server to respond
        valread = read(sfds[rIndex], buffer, 1024);
        cout << "Relay Message: " << buffer << endl;
        send(nsfd, buffer, strlen(buffer) + 1, 0);
        memset(buffer, 0, 1024);
    }
    return 0;
}

int main()
{
    vector<int> ports;
    ports.push_back(8080);
    ports.push_back(8081);
    ports.push_back(8082);
    sfds = ConnectToServers(ports);

    int PORT = 3000;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // socket file descriptor
    // getting a socket
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
    cout << "Socket listening on port " << PORT << endl;

    // blocking call
    Listen();

    // close the sockets

    return 0;
}