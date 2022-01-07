#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

int main()
{
    int pid = fork();
    if (pid > 0)
    {
        // this is parent
        cout << "This is program 1" << endl;
    }
    else
    {
        // this is the child

        // compiling the q2
        system("g++ p2.cpp -o p2");
        char **ch;

        // executing q2
        execv("p2", ch);
    }

    // waiting for child to complete
    waitpid(pid, NULL, 0);
    // deleting q2
    system("rm -f p2");
}