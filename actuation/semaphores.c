#include <stdio.h>
#include <semaphore.h>

typedef sem_t Semaphore:

Semaphore *semaphore_open(int value) { //1 for binary semaphore/mutex
    Semaphore *sem = sem_open("/servosemaphore",O_CREAT|O_EXCL, 0777, value);
    if (sem == SEM_FAILED) {
        perror("something wrong with sem_open\n");
    }
}

void semaphore_wait(Semaphore * sem) {
    int n = sem_wait(sem);
    if ( n!= 0) perror_exit("sem_wait failed!\n");
}

void semaphore_post(Semaphore * sem) {
    int n = sem_post(sem);
    if (n!= 0) perror_exit("sem_post failed\n");
}

