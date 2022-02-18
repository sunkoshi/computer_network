#include "cuds.h"
using namespace std;
int main()
{
    struct sockaddr_un addr;

    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(struct sockaddr_un));
    strncpy(addr.sun_path, "./uds.socket", sizeof(addr.sun_path) - 1);
    bind(sfd, (struct sockaddr *)&addr, sizeof(addr));

    // connect(sfd, (struct sockaddr *)&addr, sizeof(addr));
    if (connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
    {
        cout << "Error connecting to uds server..." << endl;
        exit(1);
    }
    cout << "Connected to UDS Server" << endl;
    while (true)
    {
        cout << "Waiting for train..." << endl;
        int tsfd = RecFD(sfd);
        if (tsfd <= 0)
            break;
        cout << "Received FD of train: " << tsfd << endl;
        while (true)
        {
            string s = ReadFDToString(tsfd);
            if (s == "leave" || s.size() == 0)
            {
                WriteStringToFD("left", sfd);
                cout << "Train leaving platform..." << endl;
                break;
            }
            cout << s << endl;
        }
    }
}