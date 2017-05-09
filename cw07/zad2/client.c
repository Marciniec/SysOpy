#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include "barber.h"

int main(int args, char** argv){
    sem_t *mutex;
    sem_t *barber;
    sem_t *client;
    mutex = sem_open(MUTEX,0);
    barber =sem_open(BARBER,0);
    client = sem_open(CLIENT,0);
    int chairs_id;
    int waiting_id;
    int * chairs;
    int * waiting;
    chairs_id = shm_open(CHAIRS,O_RDWR,0);
    waiting_id = shm_open(SHARED,O_RDWR,0);
    chairs  =mmap(NULL,sizeof(int),PROT_WRITE|PROT_READ,MAP_SHARED,chairs_id,0);
    waiting =mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,waiting_id,0);
    int s = atoi(argv[2]);
    int no_clients = atoi(argv[1]);
    int status;
    pid_t pid;
    int been_cut;
    struct timespec time;


    for (int i = 0; i < no_clients; ++i) {
        pid = fork();
        if(pid==0){
            been_cut =0;
            while (been_cut<s){
                sleep(1);
                clock_gettime(CLOCK_MONOTONIC,&time);
                sem_wait(mutex);
                if(*waiting<*chairs){
                    clock_gettime(CLOCK_MONOTONIC,&time);
                    printf("Customer %d is seated %ld \n", getpid(),(long)(time.tv_sec*1000000+time.tv_nsec));
                    *waiting = *waiting + 1;
                    sem_post(client);
                    sem_post(mutex);
                    been_cut++;

                } else{
                    clock_gettime(CLOCK_MONOTONIC,&time);
                    printf("Customer %d left shop %ld \n", getpid(),(long)(time.tv_sec*1000000+time.tv_nsec));
                    sem_post(mutex);
                }
            }


        }
    }

        wait(&status);
        clock_gettime(CLOCK_MONOTONIC,&time);
        printf("Customer %d left the shop being cut %d times %ld\n", getpid(), s,(long)(time.tv_sec*1000000+time.tv_nsec));
        exit(0);
    sem_close(barber);
    sem_close(mutex);
    sem_close(client);
    munmap(waiting,sizeof(int));
    munmap(chairs,sizeof(int));
    return 0;
}
