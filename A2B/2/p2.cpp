#include <unistd.h>
#include <fcntl.h>
#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    int rfd = argv[1][0];
    int wfd = argv[1][1];

    while (1)
    {
        int msgSize = 50;
        char msgIn[msgSize];
        int n = read(rfd, msgIn, msgSize);

        // if no message
        if (strlen(msgIn) == 0)
            break;

        cout << "Parent Said: " << msgIn << "\n";
        char msgOut[msgSize];
        cout << "Enter Message for Parent: ";
        cin.getline(msgOut, msgSize);

        // writing message to parent
        write(wfd, msgOut, strlen(msgOut) + 1);
        if (strlen(msgOut) == 0)
            break;
    }
    cout << "Parent exiting...\n";
    system("rm -f p2");
    exit(0);
}
