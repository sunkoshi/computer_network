#include "my_fns.h"
vector<int> judges;
bool done = false;
int mqId;
void handlePEnd(int sig)
{
    cout << "So, the performance has ended." << endl;
    cout << "Asking any one judge to ask question. " << endl;
    kill(judges[RandomInt(2)], SIGUSR2);

    cout << "Asking all the judges for score. " << endl;
    for (int i = 0; i < 3; i++)
    {
        kill(judges[i], SIGUSR1);
    }
    int totalScore = 0;
    for (int i = 0; i < 3; i++)
    {
        totalScore += atoi(ReadMSQ(mqId, 3).c_str());
    }

    cout << "Submitting score: " << totalScore << endl;
    SendMSQ(mqId, to_string(totalScore), 2);
    done = true;
}
int main()
{
    cout << "Anchor PID: " << getpid() << endl;
    signal(SIGUSR1, handlePEnd);

    int key = ftok("mq", 65);
    mqId = msgget(key, 0666);
    cout << "Enter PID of all the judges: ";
    for (int i = 0; i < 3; i++)
    {
        int x;
        cin >> x;
        judges.push_back(x);
    }

    while (!done)
        ;
}