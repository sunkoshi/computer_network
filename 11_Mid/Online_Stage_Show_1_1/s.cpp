#include "my_fns.h"

int main()
{
    int key = ftok("mqq", 65);
    int mqId = msgget(key, 0666 | IPC_CREAT);
    cout << "Enter the pid of Anchor: ";
    int anchorPid;
    cin >> anchorPid;
    string s = ReadMSQ(mqId, 2);
    // performance has ended
    kill(anchorPid, SIGUSR1);

    s = ReadMSQ(mqId, 2);
    cout << "Performer answers: " << s << endl;

    s = ReadMSQ(mqId, 2);
    cout << "Score: " << s << endl;
}