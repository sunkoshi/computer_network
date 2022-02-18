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

pthread_t reader_id, writer_id;
void *writer_thread(void *arg);
void *reader_thread(void *arg);

string msgIn_file_name = "msgOut";
string msgOut_file_name = "msgIn";
int msgIn, msgOut;

int main()
{
    mkfifo(msgIn_file_name.c_str(), 0666);
    mkfifo(msgOut_file_name.c_str(), 0666);

    pthread_create(&writer_id, NULL, writer_thread, NULL);
    pthread_create(&reader_id, NULL, reader_thread, NULL);
    pthread_join(reader_id, NULL);
    pthread_join(writer_id, NULL);

    close(msgIn);
    close(msgOut);
    return 0;
}

void *writer_thread(void *arg)
{
    // process p2 file
    cout << "Writer thread initiated\n"
         << endl;
    // write to fifo file

    msgOut = open(msgOut_file_name.c_str(), O_WRONLY);
    while (1)
    {
        cout << "Enter Message: ";
        string s = "";
        getpid();
        getline(cin, s);
        char *buff = &s[0];
        write(msgOut, buff, s.length());
    }
    close(msgOut);
}

// reads all the message
void *reader_thread(void *arg)
{
    cout << "Reader thread initiated" << endl;

    msgIn = open(msgIn_file_name.c_str(), O_RDONLY);
    while (1)
    {
        char buf[100];
        read(msgIn, buf, 100);
        string s(buf);
        cout << "\nRec: " << s << endl;
        lseek(msgIn, 0, SEEK_CUR);
    }
    close(msgIn);
}