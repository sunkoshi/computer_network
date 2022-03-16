#include "my_fh.h"
using namespace std;
#define BUF_SIZE 1024

int main()
{
    int usfd = ConnectToUdsServer(UDS_SERVER_PATH);
    int rsfd = getFD(usfd);
    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    cout << "Enter required documents: ";
    string reqDocs;
    cin >> reqDocs;
    sendto(rsfd, reqDocs.c_str(), reqDocs.size(), 0, (struct sockaddr *)&client, sizeof(client));
    sendFD(usfd, rsfd);
    close(rsfd);

    char buf[1024] = {0};
    while (1)
    {
        int nsfd = getFD(usfd);
        cout << "CLIENT: ";
        read(nsfd, buf, BUF_SIZE);
        cout << buf << endl;
        sendFD(usfd, nsfd);
        close(nsfd);
        cout << "Enter VERDICT: ";
        string ver;
        cin >> ver;
        cout << ver << endl;
        memset(buf, 0, BUF_SIZE);
    }
}