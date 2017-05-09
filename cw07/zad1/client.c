#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include "sleeepingbarber.h"
void push(pid_t pid){
    struct fifo_queue *queue;
    int queue_id;
    char * home = getenv("HOME");
    key_t queue_key = ftok(home,'t');
    queue_id=shmget(queue_key,0,0);
    queue = shmat(queue_id,NULL,0);
    queue->pids[queue->tail++]=pid;

}
pid_t pop(){
    struct fifo_queue *queue;
    int queue_id;
    char * home = getenv("HOME");
    key_t queue_key = ftok(home,'t');
    queue_id=shmget(queue_key,0,0);
    queue = shmat(queue_id,NULL,0);
    return queue->pids[queue->head++];
}
int isEmpty(){
    struct fifo_queue *queue;
    int queue_id;
    char * home = getenv("HOME");
    key_t chairs_key = ftok(home,'t');
    queue_id=shmget(chairs_key,0,0);
    queue = shmat(queue_id,NULL,0);
    return queue->head==queue->tail;
}
void up(unsigned short semaphore_id, unsigned short semaphore_num, struct sembuf *semaphore){
    semaphore->sem_num=semaphore_num;
    semaphore->sem_op =1;
    semaphore->sem_flg =0;
    semop(semaphore_id,semaphore,1);
}
void  down(unsigned short semaphore_id, unsigned short semaphore_num, struct sembuf *semaphore){

    semaphore->sem_num=semaphore_num;
    semaphore->sem_op =-1;
    semaphore->sem_flg =0;
    semop(semaphore_id,semaphore,1);
}

int main(int argv, char ** args){
    char * home = getenv("HOME");
    int no_clients = atoi(args[1]);
    int s = atoi(args[2]);
    key_t semaphore_key = ftok(home,'x');
    key_t sharem_key =ftok(home,'y');
    int semaphore_id;
    int  sharem_id;
    struct sembuf semaphore;
    sharem_id= shmget(sharem_key, 0,0666);
    semaphore_id = semget(semaphore_key,0,0666);
    int * waiting_customers;
    waiting_customers=shmat(sharem_id,NULL,0);
    int *chairs;
    int chairs_id;
    int been_cutted;
    union semnum arg;
    key_t chairs_key = ftok(home,'k');
    chairs_id=shmget(chairs_key,0,0);
    chairs = shmat(chairs_id,NULL,0);
    int status;
    pid_t pid;
    struct timespec time;
    for (int i = 0; i < no_clients; ++i) {
        pid = fork();
        if(pid == 0){
            been_cutted=0;
            while(been_cutted<s) {
                sleep(1);
                clock_gettime(CLOCK_MONOTONIC,&time);
                if(semctl(semaphore_id,CLIENTS,GETVAL,arg)==0) printf("Wakening up barber by %d  %ld\n",getpid(),(long)(time.tv_sec*1000000+time.tv_nsec));
                down((unsigned short) semaphore_id, MUTEX, &semaphore);
                if (*waiting_customers < *chairs) {
                    push(getpid());
                    clock_gettime(CLOCK_MONOTONIC,&time);
                    printf("Customer %d is seated %ld \n", getpid(),(long)(time.tv_sec*1000000+time.tv_nsec));
                    *waiting_customers = *waiting_customers + 1;
                    up((unsigned short) semaphore_id, CLIENTS, &semaphore);
                    up((unsigned short) semaphore_id, MUTEX, &semaphore);
                    been_cutted++;
                } else {
                    clock_gettime(CLOCK_MONOTONIC,&time);
                    printf("Customer %d left shop %ld \n", getpid(),(long)(time.tv_sec*1000000+time.tv_nsec));
                    up((unsigned short) semaphore_id, MUTEX, &semaphore);
                }

            }
            clock_gettime(CLOCK_MONOTONIC,&time);
            printf("Customer %d left the shop beeing cut %d times %ld\n", getpid(), s,(long)(time.tv_sec*1000000+time.tv_nsec));
            exit(0);
        }


    }
    wait(&status);
}



