#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>
#include "sleeepingbarber.h"


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
void init_semaphore(unsigned short semaphore_id, unsigned short semaphore_num, int val){
    union semnum arg;
    arg.val = val;
    semctl(semaphore_id,semaphore_num,SETVAL,arg);
}

int main(int argv, char ** args) {

    int *chairs;
    int chairs_id;
    struct timespec time;
    char * home = getenv("HOME");
    key_t chairs_key = ftok(home,'k');
    chairs_id=shmget(chairs_key,sizeof(int),IPC_CREAT|0666);
    chairs = shmat(chairs_id,NULL,0);
    *chairs = atoi(args[1]);
    key_t semaphore_key = ftok(home,'x');
    key_t sharem_key =ftok(home,'y');
    int semaphore_id;
    int  sharem_id;
    struct sembuf semaphore;
    sharem_id= shmget(sharem_key, sizeof(int),IPC_CREAT|0666);
    semaphore_id = semget(semaphore_key,3,IPC_CREAT|0666);
    int * waiting_customers;
    waiting_customers=shmat(sharem_id,NULL,0);
    *waiting_customers =0;
    int count =0;
    init_semaphore((unsigned short) semaphore_id, CLIENTS, 0);
    init_semaphore((unsigned short) semaphore_id, BARBER, 0);
    init_semaphore((unsigned short) semaphore_id, MUTEX, 1);
    printf("%d chairs at barbershop \n",*chairs);
    union semnum arg;
        while (1){
            down((unsigned short) semaphore_id, CLIENTS, &semaphore);
            down((unsigned short) semaphore_id,MUTEX,&semaphore);
            *waiting_customers = *waiting_customers-1;
            up((unsigned short) semaphore_id, BARBER, &semaphore);
            up((unsigned short) semaphore_id, MUTEX, &semaphore);
            clock_gettime(CLOCK_MONOTONIC,&time);
            printf("The barber is now cutting hair %d  %ld\n",++count,(long)(time.tv_sec*1000000+time.tv_nsec));
            sleep(6); //cutting hair
            clock_gettime(CLOCK_MONOTONIC,&time);
            printf("Barber finished cutting hair of customer %d %ld \n",count,(long)(time.tv_sec*1000000+time.tv_nsec));
            clock_gettime(CLOCK_MONOTONIC,&time);
            if (semctl(semaphore_id,CLIENTS,GETVAL,&arg)==0) printf("Barber sleeps %ld\n",(long)(time.tv_sec*1000000+time.tv_nsec));
        }

    return 0;
}