#include "my_fh.h"
using namespace std;

vector<int> nusfds;
vector<int> nsfds;

int main()
{
    int usfd = UdsServer(UDS_SERVER_PATH);
    listen(usfd, 10);
    for (int i = 0; i < 3; i++)
    {
        int nusfd = accept(usfd, NULL, NULL);
        nusfds.push_back(nusfd);
    }
    cout << "All services connected to S\n";

    int rsfd = RawSockServer(0);
    for (int i = 0; i < 3; i++)
    {
        sendFD(nusfds[i], rsfd);
        getFD(nusfds[i]);
    }
    cout << "sending rsfd\n";

    int sfd = SockServer(8000);
    listen(sfd, 10);
    while (1)
    {
        int nsfd = accept(sfd, NULL, NULL);
        nsfds.push_back(nsfd);
        for (int i = 0; i < 3; i++)
        {
            sendFD(nusfds[i], nsfd);
            getFD(nusfds[i]);
        }
    }
}