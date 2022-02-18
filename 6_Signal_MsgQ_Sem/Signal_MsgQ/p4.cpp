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
    cout << "Reaching P4 in order.\n";
    if (circularCount >= 0)
    {
        kill(rightPid, SIGUSR1);
    }
}

void reverseSignalHandler(int sig)
{
    reverseCircularCount--;
    cout << "Reaching P4 in Reverse order.\n";
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

    string pid = to_string(getpid());
    msgBuff p4pid;
    p4pid.type = 4;
    strcpy(p4pid.msg, pid.c_str());

    // putting the message into the message queue
    msgsnd(mqId, &p4pid, sizeof(p4pid), 0);

    // registering a signal handler for left process
    struct sigaction act;
    act.sa_sigaction = leftHandler;
    // flag for getting the info of the signal emitter (we can get pid using this)
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    // waiting for any signal (here in this case from p3)
    pause();
    printf("P3 PID is %d\n", leftPid);

    // receive the pid of P1 from message queue
    msgBuff msg;
    msg.type = 1;
    msgrcv(mqId, &msg, sizeof(msg), msg.type, 0);
    int rPid;
    sscanf(msg.msg, "%d", &rPid);
    // recording the pid of right side process into rightPid
    rightPid = rPid;
    printf("P1 PID is %d\n", rightPid);

    // seding the acknowledgment to p4
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