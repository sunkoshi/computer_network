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
    mkfifo("/tmp/fifo2", 0777);
    int fd = open("/tmp/fifo2", O_RDWR);
    string s = "Message from P2";
    write(fd, s.c_str(), s.size() + 1);
    cout << "Exiting P2\n";
    close(fd);
    return 0;
}