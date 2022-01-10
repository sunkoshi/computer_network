#include <unistd.h>
#include <fcntl.h>
#include <iostream>
using namespace std;

int main()
{

    // keeping a copy of rfd and wfd that has been copied to 0 and 1
    int rfd = dup(0);
    int wfd = dup(1);

    // restoring the original fd of keyboard and terminal
    dup2(open("/dev/tty", O_RDWR), 0);
    dup2(open("/dev/tty", O_RDWR), 1);

    while (1)
    {
        int msgSize = 50;
        char msgIn[msgSize];
        int n = read(rfd, msgIn, msgSize);

        // if no message
        if (strlen(msgIn) == 0)
            break;

        cout << "P1 Said: " << msgIn << "\n";
        char msgOut[msgSize];
        cout << "Enter Message for P1: ";
        cin.getline(msgOut, msgSize);

        // writing message to parent
        write(wfd, msgOut, strlen(msgOut) + 1);
        if (strlen(msgOut) == 0)
            break;
    }
    cout << "P2 exiting...\n";
    system("rm -f p3");
    exit(0);
}
