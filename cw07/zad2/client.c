#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <errno.h>
#include "barber.h"

void push(pid_t pid){
    struct fifo_queue * queue;
    int queue_id;
    queue_id = shm_open(FIFO,O_RDWR,0);
    if (queue_id == -1){
        perror("Error during shm_open for queue");
        exit(1);
    }
    queue = mmap(NULL,sizeof(struct fifo_queue),PROT_READ|PROT_WRITE,MAP_SHARED,queue_id,0);
    if (queue == (void*)-1){
        perror("Error occurred during mmap for queue");
    }
    queue->pids[queue->tail++]= pid;
}
pid_t pop(){
    struct fifo_queue * queue;
    int queue_id;
    queue_id = shm_open(FIFO,O_RDWR,0);
    if (queue_id == -1){
        perror("Error during shm_open for queue");
        exit(1);
    }
    queue = mmap(NULL,sizeof(struct fifo_queue),PROT_READ|PROT_WRITE,MAP_SHARED,queue_id,0);
    if (queue == (void*)-1){
        perror("Error occurred during mmap for queue");
    }
    return queue->pids[queue->head++];
}
int isEmpty(){
    struct fifo_queue * queue;
    int queue_id;
    queue_id = shm_open(FIFO,O_RDWR,0);
    if (queue_id == -1){
        perror("Error during shm_open for queue");
        exit(1);
    }
    queue = mmap(NULL,sizeof(struct fifo_queue),PROT_READ|PROT_WRITE,MAP_SHARED,queue_id,0);
    if (queue == (void*)-1){
        perror("Error occurred during mmap for queue");
    }
    return queue->tail == queue->head;

}

int main(int args, char** argv){
    if(args !=3){
        fprintf(stderr, "Wrong number of arguments");
        exit(1);
    }
    sem_t *mutex;
    sem_t *barber;
    sem_t *client;
    mutex = sem_open(MUTEX,0);
    if (mutex == SEM_FAILED){
        perror("Error during sem_open for mutex");
        exit(1);
    }
    barber =sem_open(BARBER,0);
    if (barber == SEM_FAILED){
        perror("Error during sem_open for barber");
        exit(1);
    }
    client = sem_open(CLIENT,0);
    if (client == SEM_FAILED){
        perror("Error during sem_open for client");
        exit(1);
    }
    int chairs_id;
    int waiting_id;
    int * chairs;
    int * waiting;
    chairs_id = shm_open(CHAIRS,O_RDWR,0);
    if (chairs_id == -1){
        perror("Error during shm_open for chairs");
        exit(1);
    }
    waiting_id = shm_open(SHARED,O_RDWR,0);
    if (waiting_id == -1){
        perror("Error during shm_open for waiting");
        exit(1);
    }
    chairs  =mmap(NULL,sizeof(int),PROT_WRITE|PROT_READ,MAP_SHARED,chairs_id,0);
    if (chairs == (void*)-1){
        perror("Error occurred during mmap for chairs");
        exit(1);
    }
    waiting =mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,waiting_id,0);
    if (waiting == (void*)-1){
        perror("Error occurred during mmap for waiting");
        exit(1);
    }
    int s = atoi(argv[2]);
    int no_clients = atoi(argv[1]);
    pid_t pid;
    int been_cut;
    struct timespec time;


    for (int i = 0; i < no_clients; ++i) {
        pid = fork();
        if(pid==0){
            been_cut =0;
            while (been_cut<s){

                clock_gettime(CLOCK_MONOTONIC,&time);
                if( sem_wait(mutex)==-1){
                    perror("Error during sem_wait(mutex)");
                    exit(1);
                }
                if(*waiting<*chairs){
                    push(getpid());
                    clock_gettime(CLOCK_MONOTONIC,&time);
                    *waiting = *waiting + 1;
                    if(*waiting == 1){
                        printf("Waking up barber by customer %d time: %ld\n",getpid(),(long)(time.tv_sec*1000000+time.tv_nsec));
                    }else {
                        printf("Customer %d is being seated  time: %ld\n", getpid(),(long)(time.tv_sec*1000000+time.tv_nsec));
                    }

                    if(sem_post(client) == -1){
                        perror("Error during sem_post(client)");
                        exit(1);
                    }
                    sleep(1);
                    if(sem_post(mutex)==-1){
                        perror("Error during sem_pos(mutex)");
                        exit(1);
                    }
                    if(sem_wait(barber)==-1){
                        perror("Error during sem_pos(barber)");
                        exit(1);
                    }
                    sleep(3);
                    been_cut++;

                } else{
                    clock_gettime(CLOCK_MONOTONIC,&time);
                    printf("Customer %d left shop due to lack of chairs time: %ld \n", getpid(),(long)(time.tv_sec*1000000+time.tv_nsec));
                    if(sem_post(mutex)==-1){
                        perror("Error during sem_pos(mutex)");
                        exit(1);
                    }
                    while (*waiting>=*chairs){
                        sleep(1);
                    }
                }
            }
            clock_gettime(CLOCK_MONOTONIC,&time);
            printf("Customer %d left the shop being cut %d times  time: %ld\n", getpid(), s,(long)(time.tv_sec*1000000+time.tv_nsec));
            exit(0);

        }
    }
    while ((pid = waitpid(-1, NULL, 0))) {
        if (errno == ECHILD) {
            break;
        }
    }

    if (sem_close(barber) == -1){
        perror("Error occurred during sem_close for barber");
        exit(1);
    }
    if(sem_close(client)==-1){
        perror("Error occurred during sem_close for client");
        exit(1);
    }
    if(sem_close(mutex) == -1){
        perror("Error occurred during sem_close for mutex");
        exit(1);
    }
    if(munmap(waiting,sizeof(int)) == -1){
        perror("Error occurred during munmap for waiting");
        exit(1);
    }
    if(munmap(chairs,sizeof(int))){
        perror("Error occurred during munmap for chairs");
        exit(1);
    }
    return 0;
}