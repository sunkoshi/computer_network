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

int main()
{
    int n;
    cout << "Enter pipe no. ";
    cin >> n;

    string pipeName = "/tmp/myfifo" + to_string(n);
    mkfifo(pipeName.c_str(), 0666);
    cout << "Created fifo pipe...\n";
    cout << "Opened fifo pipe...\n";
    while (1)
    {
        int msgOut = open(pipeName.c_str(), O_WRONLY);
        cout << "Enter Message: ";
        string s = "";
        getline(cin, s);
        write(msgOut, s.c_str(), s.length());
    }
    return 0;
}
