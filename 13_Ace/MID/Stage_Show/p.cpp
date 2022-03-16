#include "my_fns.h"
int main()
{
    int key = ftok("mqq", 65);
    int mqId = msgget(key, 0666 | IPC_CREAT);

    // letting know that performance has ended
    SendMSQ(mqId, "end", 2);
    cout << "Informed that the performance has ended..." << endl;

    // getting question
    string question = ReadMSQ(mqId, 1);
    cout << "I got the question: " << question << endl;

    // sending ans
    SendMSQ(mqId, "My name is Himanshu Sah.", 2);

    return 0;
}