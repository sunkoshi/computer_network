#include <bits/stdc++.h>
#include <signal.h>
using namespace std;

int main()
{
    int pid;
    cout << "Enter PID: ";
    cin >> pid;
    kill(pid, SIGUSR1);
}