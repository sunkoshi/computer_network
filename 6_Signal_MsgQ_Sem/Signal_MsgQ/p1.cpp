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
    cout << "Reaching P1 in order.\n";
    if (circularCount > 0)
    {
        kill(rightPid, SIGUSR1);
    }
}

void reverseSignalHandler(int sig)
{
    reverseCircularCount--;
    cout << "Reaching P1 in Reverse order.\n";
    if (reverseCircularCount > 0)
    {
        kill(leftPid, SIGUSR2);
    }
}

int main()
{
    printf("My PID is %d\n", getpid());
    // creating a message queue
    int mqId = msgget(key, 0666 | IPC_CREAT);
    msgctl(mqId, IPC_RMID, NULL);
    mqId = msgget(key, 0666 | IPC_CREAT);
    msgBuff msg;

    // receiving the message from p2
    msg.type = 2;
    msgrcv(mqId, &msg, sizeof(msg), msg.type, 0);
    int p2pid;
    // extracting p2pid from the message string
    sscanf(msg.msg, "%d", &p2pid);
    // recording the pid of right side process into rightPid
    rightPid = p2pid;
    printf("P2 PID is %d\n", rightPid);

    // sending the acknowledgment to p2
    kill(rightPid, SIGUSR1);

    // registering a signal handler for left process
    struct sigaction act;
    act.sa_sigaction = leftHandler;

    // flag for getting the info of the signal emitter (we can get pid using this)
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    // waiting for any signal (here in this case from p4)
    pause();
    printf("P4 PID is %d\n", leftPid);

    // circular signalling starts here
    cout << "Starting the circular signalling for 3 times\n";

    kill(rightPid, SIGUSR1);
    // adding the circular signalling handler
    signal(SIGUSR1, signalHandler);
    // triggering the circular signal
    // waiting for the signals
    while (circularCount)
    {
        pause();
    }

    // adding the reverse circular signalling handler
    signal(SIGUSR2, reverseSignalHandler);
    // triggering the reverse circular signal
    kill(leftPid, SIGUSR2);
    // waiting for the reverse signals
    while (reverseCircularCount)
    {
        pause();
    }
    msgctl(mqId, IPC_RMID, NULL);
}