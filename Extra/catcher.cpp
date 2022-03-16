#include <bits/stdc++.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>
using namespace std;
int cnt = 0;
void handleSingle(int sig)
{
    cout << "I am catching SIGUSR1\n";
}
int main()
{
    signal(SIGUSR2, handleSingle);
    cout << "I am catcher, My PID: " << getpid();
    int a;
    cin >> a;
}