#include "my_fh.h"
using namespace std;
#define BUF_SIZE 1024
int main()
{

    int rsfd = RawSockServer(0);
    char buff[BUF_SIZE] = {0};
    for (int i = 0; i < 3; i++)
    {
        read(rsfd, buff, BUF_SIZE);
        struct ip *ip;
        ip = (struct ip *)buff;
        cout << (buff + (ip->ip_hl) * 4) << endl;
    }
    int sfd = ConnectToTcpServer("127.0.0.1", 8000);
    cout << "Document: ";
    for (int i = 0; i < 3; i++)
    {
        string reqDocs;
        cin >> reqDocs;
        send(sfd, reqDocs.c_str(), reqDocs.size(), 0);
    }
}