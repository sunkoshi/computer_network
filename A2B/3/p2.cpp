#include <unistd.h>
#include <fcntl.h>
#include <iostream>
using namespace std;

void Proxy(int rP1, int wP1, int wP3, int rP3)
{
    while (true)
    {
        int msgSize = 50;
        // proxy to read p3
        char mP3[msgSize];
        read(rP1, mP3, msgSize);

        // send to p3
        write(wP3, mP3, strlen(mP3) + 1);
        if (strlen(mP3) == 0)
            break;

        // proxy to read p3
        char mP1[msgSize];
        read(rP3, mP1, msgSize);

        // forward to p1
        write(wP1, mP1, strlen(mP1) + 1);
        if (strlen(mP1) == 0)
            break;
    }
}

int main()
{

    int rP1 = dup(0);
    int wP1 = dup(1);

    dup2(open("/dev/tty", O_RDWR), 0);
    dup2(open("/dev/tty", O_RDWR), 1);

    int pipe1fds[2];
    int pipe2fds[2];

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
        Proxy(rP1, pipe2fds[1], pipe1fds[0], wP1);
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

        system("c++ p3.cpp -o p3");
        char **ch;
        execv("p3", ch);
    }

    cout << "P2 exiting..." << endl;
    system("rm -f p2");
}
