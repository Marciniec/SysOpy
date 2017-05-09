#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <asm/errno.h>
#include <errno.h>
#include "sleeepingbarber.h"
char * home;
void push(pid_t pid){
    struct fifo_queue *queue;
    int queue_id;
    key_t queue_key = ftok(home,'t');
    if (queue_key==-1){
        perror("Error occurred during creating queue key");
        exit(1);
    }
    queue_id=shmget(queue_key,0,0);
    if (queue_id == -1) {
        perror("Error occurred during shmget for queue");
        exit(1);
    }
    queue = shmat(queue_id,NULL,0);
    if (queue == (void*)-1){
        perror("Error occurred during shmat for queue");
        exit(1);
    }
    queue->pids[queue->tail++]=pid;

}
pid_t pop(){
    struct fifo_queue *queue;
    int queue_id;
    key_t queue_key = ftok(home,'t');
    if (queue_key==-1){
        perror("Error occurred during creating queue key");
        exit(1);
    }
    queue_id=shmget(queue_key,0,0);
    if (queue_id == -1) {
        perror("Error occurred during shmget for queue");
        exit(1);
    }
    queue = shmat(queue_id,NULL,0);
    if (queue == (void*)-1){
        perror("Error occurred during shmat for queue");
        exit(1);
    }
    return queue->pids[queue->head++];
}
int isEmpty(){
    struct fifo_queue *queue;
    int queue_id;
    key_t queue_key = ftok(home,'t');
    if (queue_key==-1){
        perror("Error occurred during creating queue key");
        exit(1);
    }
    queue_id=shmget(queue_key,0,0);
    if (queue_id == -1) {
        perror("Error occurred during shmget for queue");
        exit(1);
    }
    queue = shmat(queue_id,NULL,0);
    if (queue == (void*)-1){
        perror("Error occurred during shmat for queue");
        exit(1);
    }
    return queue->head==queue->tail;
}

void up(int semaphore_id, unsigned short semaphore_num, struct sembuf *semaphore){
    semaphore->sem_num=semaphore_num;
    semaphore->sem_op =1;
    semaphore->sem_flg =0;
    if(semop(semaphore_id,semaphore,1)==-1){
        perror("Error occurred during semop in up");
        exit(1);
    }
}
void  down(int semaphore_id, unsigned short semaphore_num, struct sembuf *semaphore){
    semaphore->sem_num=semaphore_num;
    semaphore->sem_op =-1;
    semaphore->sem_flg =0;
    if(semop(semaphore_id,semaphore,1)==-1){
        perror("Error occurred during semop in up");
        exit(1);
    }
}
int main(int argv, char ** args){
    if (argv !=3){
        fprintf(stderr,"Wrong number of arguments");
        exit(1);
    }
    home = getenv("HOME");
    int no_clients = atoi(args[1]);
    int s = atoi(args[2]);
    key_t semaphore_key = ftok(home,'x');
    if (semaphore_key == -1){
        perror("Error during getting key for semaphore");
        exit(1);
    }
    key_t sharem_key =ftok(home,'y');
    if(sharem_key == -1){
        perror("Error during getting key for sharem");
        exit(1);
    }
    int semaphore_id;
    int  sharem_id;
    struct sembuf semaphore;
    sharem_id= shmget(sharem_key, 0,0666);
    if (sharem_id == -1) {
        perror("Error occurred during shmget for sharem");
        exit(1);
    }
    semaphore_id = semget(semaphore_key,0,0666);
    if (semaphore_id == -1){
        perror("Error occurred during semget for semaphore");
        exit(1);
    }
    int * waiting_customers;
    waiting_customers=shmat(sharem_id,NULL,0);
    if (waiting_customers == (void*)-1){
        perror("Error occurred during shmat for waiting customers");
        exit(1);
    }
    int *chairs;
    int chairs_id;
    int been_cutted;
    union semnum arg;
    key_t chairs_key = ftok(home,'k');
    if (chairs_key == -1){
        perror("Error during getting key for chairs");
        exit(1);
    }
    chairs_id=shmget(chairs_key,0,0);
    if (chairs_id == -1){
        perror("Error occurred during shmget for chairs");
        exit(1);
    }
    chairs = shmat(chairs_id,NULL,0);
    if (chairs == (void*)-1){
        perror("Error occurred during shmat for chairs");
        exit(1);
    }
    pid_t pid;
    struct timespec time;
    for (int i = 0; i < no_clients; ++i) {
         pid = fork();
        if(pid == 0){
            been_cutted=0;
            while(been_cutted<s) {
                clock_gettime(CLOCK_MONOTONIC,&time);
                down(semaphore_id, MUTEX, &semaphore);
                if (*waiting_customers < *chairs) {
                    push(getpid());
                    clock_gettime(CLOCK_MONOTONIC,&time);
                    *waiting_customers = *waiting_customers + 1;
                    if(*waiting_customers==1){
                        printf("Waking up barber by %d\n",getpid());
                    } else {
                        printf("Customer %d is seated in waiting room time: %ld \n", getpid(), (long) (time.tv_sec * 1000000 + time.tv_nsec));
                    }
                    up( semaphore_id, CLIENTS, &semaphore);
                    sleep(1);
                    up( semaphore_id, MUTEX, &semaphore);
                    down(semaphore_id, BARBER, &semaphore);
                    sleep(3);
                    been_cutted++;
                } else {
                    clock_gettime(CLOCK_MONOTONIC,&time);
                    printf("Customer %d left shop due to lack of chairs time: %ld \n", getpid(),(long)(time.tv_sec*1000000+time.tv_nsec));

                    up( semaphore_id, MUTEX, &semaphore);
                    while (*waiting_customers>=*chairs){
                        sleep(1);
                    }
                }

            }
            clock_gettime(CLOCK_MONOTONIC,&time);
            printf("Customer %d left the shop being cut %d times %ld\n", getpid(), s,(long)(time.tv_sec*1000000+time.tv_nsec));
            exit(0);
        } else{

        }


    }
    while ((pid = waitpid(-1, NULL, 0))) {
        if (errno == ECHILD) {
            break;
        }
    }
    return 0;
}



