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

string msgIn_file_name = "msgIn";
string msgOut_file_name = "msgOut";
int msgIn, msgOut;
int main()
{
    mkfifo(msgIn_file_name.c_str(), 0666);
    mkfifo(msgOut_file_name.c_str(), 0666);

    cout << "Server started...";
    msgIn = open(msgIn_file_name.c_str(), O_RDONLY);
    msgOut = open(msgOut_file_name.c_str(), O_WRONLY);
    while (1)
    {
        lseek(msgIn, 0, SEEK_CUR);
        char buf[100];
        read(msgIn, buf, 100);
        cout << buf << endl;
        write(msgOut, buf, strlen(buf));
    }
    close(msgOut);
    close(msgIn);
    return 0;
}
