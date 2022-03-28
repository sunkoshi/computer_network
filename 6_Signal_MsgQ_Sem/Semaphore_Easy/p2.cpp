#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

const char *s12name = "./s12";
const char *s21name = "./s21";

int main()
{
    sem_t *S12 = sem_open(s12name, O_EXCL);
    sem_t *S21 = sem_open(s21name, O_EXCL);

    cout << "I am P2, I am waiting for signal in S12...\n";
    sem_wait(S12);
    cout << "Got signal in S12...\n";
    cout << "Do you want to send signal in s21? y/n \n";
    char ch;
    cin >> ch;
    cout << "Now I am sending signal in S21\n";
    sem_post(S21);
    sem_unlink(s21name);
    return 0;
}