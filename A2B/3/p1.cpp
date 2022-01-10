#include <unistd.h>
#include <iostream>
using namespace std;

void InParent(int rfd, int wfd)
{
    while (1)
    {
        int msgSize = 50;
        char msgOut[msgSize];
        cout << "Enter Message for P3: ";
        cin.getline(msgOut, msgSize);

        // writing message to child
        write(wfd, msgOut, strlen(msgOut) + 1);

        // exiting if message is empty
        if (strlen(msgOut) == 0)
            break;

        char msgIn[msgSize];
        read(rfd, msgIn, msgSize);
        if (strlen(msgIn) == 0)
            break;
        cout << "P3 Said: " << msgIn << endl;
    }

    cout << "Child exiting...\n";
    exit(0);
}

int main()
{
    // index 0 -> Read FD
    // index 1 -> Write FD
    int pipe1fds[2], pipe2fds[2];

    // this will open a pipe and assign the FDs to 0 and 1
    pipe(pipe1fds);
    pipe(pipe2fds);

    int pid = fork();
    if (pid > 0)
    {
        // in parent

        // closing the writer of pipe 1
        close(pipe1fds[1]);

        // closing the reader of pipe 2
        close(pipe2fds[0]);
        InParent(pipe1fds[0], pipe2fds[1]);
    }
    else
    {
        // in child

        // closing the reader of pipe 1
        close(pipe1fds[0]);

        // closing the writer of pipe 2
        close(pipe2fds[1]);

        // mapping the read fd to read source, i.e. keyboard
        dup2(pipe2fds[0], 0);
        // mapping the write fd to write source, i.e. terminal
        dup2(pipe1fds[1], 1);

        system("c++ p2.cpp -o p2");
        char **ch;
        execv("p2", ch);
    }
}