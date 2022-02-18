#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <bits/stdc++.h>
using namespace std;
// input 2 3 5 4 7 2
// output 5 1 14
int main()
{
    int a, b;
    cin >> a >> b;
    cout << a + b << " ";
    cin >> a >> b;
    cout << a - b << " ";
    cin >> a >> b;
    cout << a * b;
}