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
    sem_t *S12 = sem_open(s12name, O_CREAT | O_EXCL, 0666, 0);
    sem_t *S21 = sem_open(s21name, O_CREAT | O_EXCL, 0666, 0);

    S12 = sem_open(s12name, O_EXCL);
    S21 = sem_open(s21name, O_EXCL);

    cout << "Do you want to send signal in S12? y/n \n";
    char ch;
    cin >> ch;
    sem_post(S12);
    cout << "Now I am waiting for signal in s21\n";
    sem_wait(S21);
    cout << "Got signal in S21...";
    sem_unlink(s12name);
    return 0;
}