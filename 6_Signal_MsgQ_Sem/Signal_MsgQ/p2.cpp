#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

struct msgBuff
{
    long type;
    char msg[128];
};

int leftPid, rightPid;

int circularCount = 3, reverseCircularCount = 3;

key_t key = ftok("mqq", 65);

void leftHandler(int sig, siginfo_t *info, void *context)
{
    leftPid = info->si_pid;
}

void signalHandler(int sig)
{
    circularCount--;
    cout << "Reaching P2 in order.\n";
    if (circularCount >= 0)
    {
        kill(rightPid, SIGUSR1);
    }
}

void reverseSignalHandler(int sig)
{
    reverseCircularCount--;
    cout << "Reaching P2 in Reverse order.\n";
    if (reverseCircularCount >= 0)
    {
        kill(leftPid, SIGUSR2);
    }
}

int main()
{
    printf("My PID is %d\n", getpid());
    // creating a message queue
    int mqId = msgget(key, 0666 | IPC_CREAT);

    // putting the message into the message queue
    string pid = to_string(getpid());
    msgBuff p2pid;
    p2pid.type = 2;
    strcpy(p2pid.msg, pid.c_str());
    msgsnd(mqId, &p2pid, sizeof(p2pid), 0);

    // registering a signal handler for left process
    struct sigaction act;
    act.sa_sigaction = leftHandler;
    // flag for getting the info of the signal emitter (we can get pid using this)
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    // waiting for any signal (here in this case from p1)
    pause();
    printf("P1 PID is %d\n", leftPid);

    // receive the pid of P3 from message queue
    msgBuff msg;
    msg.type = 3;
    msgrcv(mqId, &msg, sizeof(msg), msg.type, 0);
    int rPid;
    sscanf(msg.msg, "%d", &rPid);
    // recording the pid of right side process into rightPid
    rightPid = rPid;
    printf("P3 PID is %d\n", rightPid);

    // sending the pid of P1 into the message queue
    msgBuff p1pid;
    p1pid.type = 1;
    strcpy(p1pid.msg, to_string(leftPid).c_str());
    // putting the pid of P1 in the message queue
    msgsnd(mqId, &p1pid, sizeof(p1pid), 0);

    // seding the acknowledgment to p3
    kill(rightPid, SIGUSR1);

    // adding the circular signalling handler
    signal(SIGUSR1, signalHandler);
    // waiting for the signals
    while (circularCount)
    {
        pause();
    }

    // adding the reverse circular signalling handler
    signal(SIGUSR2, reverseSignalHandler);
    // waiting for the reverse signals
    while (reverseCircularCount)
    {
        pause();
    }
}