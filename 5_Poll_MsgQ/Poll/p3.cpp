#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
using namespace std;

int main()
{
    mkfifo("/tmp/fifo3", 0777);
    int fd = open("/tmp/fifo3", O_RDWR);
    string s = "Message from P3";
    write(fd, s.c_str(), s.size() + 1);
    cout << "Exiting P3\n";
    close(fd);
    return 0;
}