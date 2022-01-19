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
        read(rfd, msgIn, msgSize);
        cout << "In P3 from P1: " << msgIn << endl;
        // if no message
        if (strlen(msgIn) == 0)
        {
            cout << "Empty message from P1, exiting...";
            break;
        }
        char msgOut[msgSize];
        cout << "Enter Message for P1: ";
        cin.getline(msgOut, msgSize);

        // writing message to parent
        write(wfd, msgOut, strlen(msgOut) + 1);
        if (strlen(msgOut) == 0)
            break;
    }
    cout << "P3 exiting...\n";
    exit(0);
}
