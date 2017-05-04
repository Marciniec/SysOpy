#include <stdio.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#include "barber.h"
sem_t *barber;
sem_t *client;
sem_t *mutex;
int  * waiting;
int * chairs;
void hadler(int sig){
    if(sig ==SIGINT){
        sem_close(barber);
        sem_close(client);
        sem_close(mutex);
        munmap(waiting,sizeof(int));
        munmap(chairs,sizeof(int));
        sem_unlink(BARBER);
        sem_unlink(CLIENT);
        sem_unlink(MUTEX);
        shm_unlink(CHAIRS);
        shm_unlink(SHARED);
        exit(0);
    }
}


int main(int args, char** argv) {
    if(args!=2){
        printf("Wrong number of arguments\n");
        exit(1);
    }
    if(( barber = sem_open(BARBER,O_CREAT ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,0)) ==SEM_FAILED){
        perror("Error during opening barber semaphore");
        exit(1);
    }
    if((client = sem_open(CLIENT, O_CREAT ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,0) )== SEM_FAILED){
        perror("Error during opening client semaphore");
        exit(1);
    }
    if(( mutex = sem_open(MUTEX,O_CREAT ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,1)) == SEM_FAILED){
        perror("Error during opening mutex semaphore");
        exit(1);
    }

    int shared_des = shm_open(SHARED,O_CREAT | O_RDWR,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if(shared_des ==-1){
        perror("Error during opening shared memory(waiting)");
        exit(1);
    }
    struct timespec time;
    if( ftruncate(shared_des,sizeof(int))==-1){
        perror("ftruncate waiting");
        exit(1);
    }

    waiting = mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,shared_des,0);
    if(waiting==MAP_FAILED){
        perror("mmap waiting");
        exit(1);
    }
    *waiting =0;
    int count = 0;
    int chair_des = shm_open(CHAIRS,O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if(chair_des ==-1){
        perror("Error during opening shared memory(chairs)");
        exit(1);
    }
   if( ftruncate(chair_des,sizeof(int))==-1){
       perror("ftruncate chairs");
       exit(1);
   }

    chairs = mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,chair_des,0);
    *chairs= atoi(argv[1]);
    if(chairs==MAP_FAILED){
        perror("mmap waiting");
        exit(1);
    }
    int value;
    signal(SIGINT,hadler);
    while (1){
       if( sem_wait(client)==-1){
           perror("Error during sem_wait(client)");
           exit(1);
       }
        if( sem_wait(mutex)==-1){
            perror("Error during sem_wait(client)");
            exit(1);
        }
        *waiting=*waiting-1;
        if(sem_post(barber)==-1){
            perror("Error during sem_pos(barber)");
            exit(1);
        }
        if(sem_post(mutex)==-1){
            perror("Error during sem_pos(mutex)");
            exit(1);
        }
        clock_gettime(CLOCK_MONOTONIC,&time);
        printf("The barber is now cutting hair %d  %ld\n",++count,(long)(time.tv_sec*1000000+time.tv_nsec));
        clock_gettime(CLOCK_MONOTONIC,&time);
        printf("Barber finished cutting hair of customer %d %ld \n",count,(long)(time.tv_sec*1000000+time.tv_nsec));
        if(sem_getvalue(client,&value)==-1){
            perror("Error during (sem_getvalue(client,&value)");
            exit(1);
        }
        clock_gettime(CLOCK_MONOTONIC,&time);
        if(value ==0) printf("Barber is sleeping %ld \n",(long)(time.tv_sec*1000000+time.tv_nsec));
    }


    return 0;
}