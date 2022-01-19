#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
using namespace std;

struct msg_st
{
    long int type;
    char msg_text[128];
};

int main()
{
    int key = ftok("mq", 65);
    int mqId = msgget(key, 0666 | IPC_CREAT);

    msg_st msg;
    for (int i = 0; i < 3; i++)
    {
        msg_st msg;
        msg.type = 1;
        msgrcv(mqId, &msg, sizeof(msg), msg.type, 0);
        cout << "In P1: " << msg.msg_text << endl;

        msg.type = i + 2;
        strcat(msg.msg_text, " - From P1");
        msgsnd(mqId, &msg, sizeof(msg), msg.type);
    }
    msgctl(mqId, IPC_RMID, NULL);
    return 0;
}