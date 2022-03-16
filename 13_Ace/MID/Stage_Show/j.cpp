#include "my_fns.h"
int mqId;
bool done = false;
void handleAskQue(int sig)
{
    cout << "Asking Question: What is your name? " << endl;
    SendMSQ(mqId, "What is your name?", 1);
}
void handleGiveScore(int sig)
{
    SendMSQ(mqId, to_string(RandomInt(100)), 3);
    done = true;
}
int main()
{
    cout << "Judge PID: " << getpid() << endl;
    signal(SIGUSR2, handleAskQue);
    signal(SIGUSR1, handleGiveScore);

    int key = ftok("mq", 65);
    mqId = msgget(key, 0666);
    while (!done)
        ;
}