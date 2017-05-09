#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include "sleeepingbarber.h"
char * home;

void handler(int sig){
    struct fifo_queue *queue;
    int queue_id;
    key_t queue_key = ftok(home,'t');
    queue_id=shmget(queue_key,0,0);
    queue = shmat(queue_id,NULL,0);


    if(shmdt( queue)==-1){
        perror("shmdt ");
        exit(1);
    }

    struct shmid_ds * buf =NULL;
    shmctl(queue_id, IPC_RMID,buf);
    key_t semaphore_key = ftok(home,'x');
    if (semaphore_key == -1){
        perror("Error during getting key for semaphore");
        exit(1);
    }
    int semaphore_id = semget(semaphore_key,0,0666);
    if (semaphore_id == -1){
        perror("Error occurred during semget for semaphore");
        exit(1);
    }
    if(semctl(semaphore_id,0,IPC_RMID) == -1){
        perror("Error occurred during semctl");
        exit(1);
    }

    exit(0);

}

void init_queue(){
    struct fifo_queue *queue;
    int queue_id;
    key_t queue_key = ftok(home,'t');
    if (queue_key==-1){
        perror("Error occurred during creating queue key");
        exit(1);
    }
    queue_id=shmget(queue_key,sizeof(struct fifo_queue),IPC_CREAT|IPC_EXCL|0666);
    if (queue_id == -1) {
        perror("Error occurred during shmget for queue");
        exit(1);
    }
    queue = shmat(queue_id,NULL,0);
    if (queue == (void*)-1){
        perror("Error occurred during shmat for queue");
        exit(1);
    }
    queue->head=0;
    queue->tail=0;
}
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
void init_semaphore(int semaphore_id, unsigned short semaphore_num, int val){
    union semnum arg;
    arg.val = val;
   if ( semctl(semaphore_id,semaphore_num,SETVAL,arg)== -1){
       perror("Error occurred during semctl");
       exit(1);
   }
}

int main(int argv, char ** args) {
    if (argv != 2){
        fprintf(stderr,"Wrong number of arguments");
        exit(1);
    }
    int *chairs;
    int chairs_id;
    struct timespec time;
    home = getenv("HOME");
    if (home == NULL){
        fprintf(stderr,"Error occurred during getting HOME variable");
        exit(1);
    }
    key_t chairs_key = ftok(home,'k');
    if (chairs_key == -1){
        perror("Error during getting key for chairs");
        exit(1);
    }
    chairs_id=shmget(chairs_key,sizeof(int),IPC_CREAT|0666);
    if (chairs_id == -1){
        perror("Error occurred during shmget for chairs");
        exit(1);
    }
    chairs = shmat(chairs_id,NULL,0);
    if (chairs == (void*)-1){
        perror("Error occurred during shmat for chairs");
        exit(1);
    }
    *chairs = atoi(args[1]);
    key_t semaphore_key = ftok(home,'x');
    if (semaphore_key == -1){
        perror("Error during getting key for semaphore");
        exit(1);
    }
    key_t sharem_key =ftok(home,'y');
    if (sharem_key == -1){
        perror("Error during getting key for shm");
        exit(1);
    }
    int semaphore_id;
    int  sharem_id;
    struct sembuf semaphore;
    sharem_id= shmget(sharem_key, sizeof(int),IPC_CREAT|0666);
    if (sharem_id == -1) {
        perror("Error occurred during shmget for sharem");
        exit(1);
    }
    semaphore_id = semget(semaphore_key,3,IPC_CREAT|0666);
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
    *waiting_customers =0;

    init_semaphore( semaphore_id, CLIENTS, 0);
    init_semaphore( semaphore_id, BARBER, 0);
    init_semaphore( semaphore_id, MUTEX, 1);
    printf("%d chairs at barbershop \n",*chairs);
    init_queue();
    int pid=0;
    union semnum arg;
    signal(SIGINT,handler);

        while (1){
            down((unsigned short) semaphore_id, CLIENTS, &semaphore);
            down((unsigned short) semaphore_id,MUTEX,&semaphore);
            *waiting_customers = *waiting_customers-1;
            up((unsigned short) semaphore_id, BARBER, &semaphore);
            up((unsigned short) semaphore_id, MUTEX, &semaphore);
            clock_gettime(CLOCK_MONOTONIC,&time);
            if(!isEmpty()) {
                pid= pop();
            }
            printf("The barber is now cutting hair %d  %ld\n",pid,(long)(time.tv_sec*1000000+time.tv_nsec));
            sleep(6); //cutting hair
            clock_gettime(CLOCK_MONOTONIC,&time);
            printf("Barber finished cutting hair of customer %d %ld \n",pid,(long)(time.tv_sec*1000000+time.tv_nsec));
            clock_gettime(CLOCK_MONOTONIC,&time);
            if (semctl(semaphore_id,CLIENTS,GETVAL,&arg)==0) printf("Barber sleeps %ld\n",(long)(time.tv_sec*1000000+time.tv_nsec));
        }


    return 0;
}