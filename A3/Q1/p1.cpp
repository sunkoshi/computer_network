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
int file_fd;

void *writer_thread(void *arg);
void *reader_thread(void *arg);
string file_name = "demo";

int main()
{
    // compiling both the files
    system("g++ p2.cpp -o p2");
    system("g++ p3.cpp -o p3");

    // executing p2 with popen with write option
    FILE *fd = popen("./p2", "w");

    // getting the file_fd using fileno for writing to it
    file_fd = fileno(fd);

    pthread_create(&writer_id, NULL, writer_thread, NULL);
    pthread_create(&reader_id, NULL, reader_thread, NULL);
    pthread_join(reader_id, NULL);
    pthread_join(writer_id, NULL);

    // removing demo pipe file
    system("rm -f ./demo");
    return 0;
}

void *writer_thread(void *arg)
{
    // process p2 file
    cout << "Writer thread initiated" << endl;
    // write to fifo file

    while (1)
    {
        cout << "Enter a string to capitalise: ";
        char buf[100];
        cin.getline(buf, 100);
        // write to file_fd obtained after executing p2 using popen
        write(file_fd, buf, strlen(buf) + 1);
        if (strcmp(buf, "exit") == 0)
        {
            cout << "Writer thread exiting" << endl;
            return 0;
        }
    }
    close(file_fd);
}

void *reader_thread(void *arg)
{
    // the reader is going to read from the named pipe or fifo so, mkfifo has to be used

    // fifo file
    cout << "Reader thread initiated" << endl;

    // fifo file
    int fifo_fd;

    // open(and create also if file not there) fifo file with name as file_name
    if ((fifo_fd = mkfifo(file_name.c_str(), 0666)) == -1)
    {
        perror("mkfifo");
        exit(1);
    }

    // opening the fifo file in readonly
    fifo_fd = open(file_name.c_str(), O_RDONLY);

    // read from fifo file
    while (1)
    {
        char buf[100];
        read(fifo_fd, buf, 100);
        cout << "\nFinally: " << buf << endl;
        if (strcmp(buf, "exit") == 0)
        {
            cout << "Reader thread exiting" << endl;
            return 0;
        }
    }
    close(fifo_fd);
}