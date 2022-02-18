#include "cuds.h"
using namespace std;

int main()
{
    // create a UDS server
    int udsSfd = CreateUDSServer("./uds.socket", true);
    int sSfd = CreateSocketServer(8080);
    listen(udsSfd, 100);
    listen(sSfd, 100);
    // listening to both the server
    // AsyncListenSocketServer(sSfd);
    // AsyncListenUDSServer(udsSfd);

    vector<int> serverSfds;
    serverSfds.push_back(udsSfd);
    serverSfds.push_back(sSfd);

    PollListenToServers(serverSfds);

    //     while (true)
    // {
    // }
    // while (true)
    // {
    //     // accept
    //     int conn = accept(sfd, NULL, NULL);
    //     SendFD(conn, fd);
    //     close(conn);
    // }
}