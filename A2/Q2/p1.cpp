#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

int main()
{
    int pid1 = fork();
    int pid2 = fork();
    if (pid1 == 0)
    {
        system("g++ p2.cpp -o p2");
        char **ch;
        execv("p2", ch);
    }
    if (pid2 == 0)
    {
        system("g++ p3.cpp -o p3");
        char **ch;
        execv("p3", ch);
    }
    int res = waitpid(pid1, NULL, 0);
    cout << res << " Program 2 quitting" << endl;
    cout << waitpid(pid2, NULL, 0) << " Program 3 quitting" << endl;

    if (res > 0)
    {
        cout << "This statement will only run Program 2 quits";
    }

    // deleting q2
    system("rm -f p2");
    system("rm -f p3");
}
