#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
using namespace std;

int main()
{
    cout << "Starting p2\n";
    FILE *fd = popen("./p3", "w");
    int file_fd = fileno(fd);
    dup2(file_fd, 1);
    while (1)
    {
        char buff[100];
        read(0, buff, 100);
        lseek(0, 0, SEEK_CUR);
        write(1, buff, strlen(buff) + 1);
        if (strcmp(buff, "exit") == 0)
        {
            break;
        }
    }
}