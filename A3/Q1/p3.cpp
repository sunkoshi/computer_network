#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
using namespace std;

string file_name = "demo";

int main()
{
    cout << "Starting p3\n";
    int fifo_fd = open(file_name.c_str(), O_WRONLY);
    while (1)
    {

        // declaring buffer
        char buff[100];

        // reading from 0
        read(0, buff, 100);
        lseek(0, 0, SEEK_CUR);
        // if exit is passed, then exit
        if (strcmp("exit", buff) == 0)
        {
            write(fifo_fd, buff, strlen(buff) + 1);
            cout << "Exiting P3";
            return 0;
        }
        cout << "Received in Capitalizer: " << buff << endl;
        // capitalizing
        for (int i = 0; buff[i] != '\0'; i++)
        {
            if (buff[i] >= 'a' && buff[i] <= 'z')
            {
                buff[i] += ('A' - 'a');
            }
        }
        string msg(buff);
        write(fifo_fd, msg.c_str(), msg.length() + 1);
    }
    close(fifo_fd);
}