#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <iostream>
using namespace std;
int main()
{
    mkfifo("/tmp/fifo2", 0666);
    mkfifo("/tmp/fifo3", 0666);
    mkfifo("/tmp/fifo4", 0666);

    int size = 3;
    struct pollfd pfd[size];

    pfd[0].fd = open("/tmp/fifo2", O_RDWR);
    pfd[0].events = POLLIN;
    pfd[0].revents = 0;

    pfd[1].fd = open("/tmp/fifo3", O_RDWR);
    pfd[1].events = POLLIN;
    pfd[1].revents = 0;

    pfd[2].fd = open("/tmp/fifo4", O_RDWR);
    pfd[2].events = POLLIN;
    pfd[2].revents = 0;

    cout << "Starting Poll" << endl;
    while (1)
    {
        cout << "Poll started with timeout at -1" << endl;
        int status = poll(pfd, size, -1);
        if (status == 0)
        {
            cout << "Poll timed out.." << endl;
            continue;
        }
        else if (status < 0)
        {
            cout << "poll failed..." << endl;
            perror("poll");
            exit(1);
        }
        for (int i = 0; i < size; i++)
        {
            if (pfd[i].revents & POLLIN)
            {
                char buff[128];
                read(pfd[i].fd, buff, 128);
                cout << buff << endl;
                pfd[i].revents = 0;
            }
        }
    }

    for (int i = 0; i < size; i++)
    {
        close(pfd[i].fd);
    }
    cout << "Exiting poll" << endl;
    return 0;
}