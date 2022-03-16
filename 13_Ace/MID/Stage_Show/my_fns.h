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
#include <signal.h>
#include <bits/stdc++.h>
using namespace std;
struct msg_st
{
    long int type;
    char msg_text[128];
};

void SendMSQ(int mqId, string s, int type)
{
    msg_st msg;
    msg.type = type;
    strcpy(msg.msg_text, s.c_str());
    msgsnd(mqId, &msg, sizeof(msg.msg_text), 0);
}
string ReadMSQ(int mqId, int type)
{
    msg_st msg;
    msgrcv(mqId, &msg, sizeof(msg.msg_text), type, 0);
    return string(msg.msg_text);
}
int RandomInt(int max)
{
    return (rand() % max) + 1;
}
