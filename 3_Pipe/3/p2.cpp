#include <unistd.h>
#include <fcntl.h>
#include <iostream>
using namespace std;

void Proxy(int fromP1, int toP1, int fromP3, int toP3)
{
    while (true)
    {
        int msgSize = 50;
        // proxy to read p3
        char mP3[msgSize];
        read(fromP1, mP3, msgSize);
        cout << "In P2 from P1 Proxy: " << mP3 << endl;
        // send to p3
        write(toP3, mP3, strlen(mP3) + 1);
        if (strlen(mP3) == 0)
            break;

        // proxy to read p3
        char mP1[msgSize];
        read(fromP3, mP1, msgSize);
        cout << "In P2 from P3 Proxy: " << mP1 << endl;

        // forward to p1
        write(toP1, mP1, strlen(mP1) + 1);
        if (strlen(mP1) == 0)
            break;
    }
    cout << "P2 exiting..." << endl;
    exit(0);
}

int main(int argc, char **argv)
{

    system("g++ p3.cpp -o p3");
    int rP1 = argv[1][0];
    int wP1 = argv[1][1];
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
        Proxy(rP1, wP1, pipe1fds[0], pipe2fds[1]);
    }
    else
    {
        // in child

        // closing the reader of pipe 1
        close(pipe1fds[0]);

        // closing the writer of pipe 2
        close(pipe2fds[1]);

        char fds[2];
        fds[0] = pipe2fds[0];
        fds[1] = pipe1fds[1];

        char *argv[] = {"./p3", fds, NULL};
        execv("./p3", argv);
    }
}
