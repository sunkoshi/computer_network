#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

key_t smkeyx = ftok("./smx_file", 65);
key_t smkeyy = ftok("./smy_file", 65);

const char *semx = "s1";
const char *semy = "s2";

int main()
{
    int smidx = shmget(smkeyx, 16, 0666 | IPC_CREAT);
    int smidy = shmget(smkeyy, 16, 0666 | IPC_CREAT);

    int *x = (int *)shmat(smidx, 0, 0);
    int *y = (int *)shmat(smidy, 0, 0);

    sem_t *S1 = sem_open(semx, O_CREAT | O_EXCL, 0666, 0);
    sem_t *S2 = sem_open(semy, O_CREAT | O_EXCL, 0666, 0);

    *x = 1;
    *y = 1;

    cout << "Before: "
         << "x = " << *x << " and y = " << *y << endl;

    sem_post(S1);

    cout << "Waiting for S2\n";
    sem_wait(S2);

    cout << "After: "
         << "x = " << *x << " and y = " << *y << endl;

    shmdt(x);
    shmdt(y);

    shmctl(smidx, IPC_RMID, NULL);
    shmctl(smidy, IPC_RMID, NULL);

    sem_unlink(semx);
    sem_unlink(semy);
    return 0;
}