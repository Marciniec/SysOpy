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
struct fifo_queue * queue;
void init(){
    int queue_id;
    queue_id = shm_open(FIFO,O_CREAT | O_RDWR,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (queue_id == -1){
        perror("Error during shm_open for queue");
        exit(1);
    }
    if( ftruncate(queue_id,sizeof(struct fifo_queue))==-1){
        perror("Error during ftruncate for queue");
        exit(1);
    }
    queue = mmap(NULL,sizeof(struct fifo_queue),PROT_READ|PROT_WRITE,MAP_SHARED,queue_id,0);
    if (queue == (void*)-1){
        perror("Error occurred during mmap for queue");
        exit(1);
    }
    queue->head = 0;
    queue->tail = 0;
}
void push(pid_t pid){
    int queue_id;
    queue_id = shm_open(FIFO,O_RDWR,0);
    if (queue_id == -1){
        perror("Error during shm_open for queue");
        exit(1);
    }
    queue = mmap(NULL,sizeof(struct fifo_queue),PROT_READ|PROT_WRITE,MAP_SHARED,queue_id,0);
    if (queue == (void*)-1){
        perror("Error occurred during mmap for queue");
        exit(1);
    }
    queue->pids[queue->tail++]= pid;
}
pid_t pop(){
    int queue_id;
    queue_id = shm_open(FIFO,O_RDWR,0);
    if (queue_id == -1){
        perror("Error during shm_open for queue");
        exit(1);
    }
    queue = mmap(NULL,sizeof(struct fifo_queue),PROT_READ|PROT_WRITE,MAP_SHARED,queue_id,0);
    if (queue == (void*)-1){
        perror("Error occurred during mmap for queue");
        exit(1);
    }
    return queue->pids[queue->head++];
}
int isEmpty(){
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
void hadler(int sig){
    if(sig ==SIGINT){
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
        if(munmap(queue,sizeof(struct fifo_queue)) == -1){
            perror("Error occurred during munmap for queue");
            exit(1);
        }
        if(sem_unlink(BARBER) ==-1){
            perror("Error occurred during sem_unlink for barber");
            exit(1);
        }
        if(sem_unlink(CLIENT) ==-1){
            perror("Error occurred during sem_unlink for client");
            exit(1);
        }
        if(sem_unlink(MUTEX) ==-1){
            perror("Error occurred during sem_unlink for mutex");
            exit(1);
        }
        if(shm_unlink(CHAIRS) == -1){
            perror("Error occurred during shm_unlink for chairs ");
            exit(1);
        }
        if(shm_unlink(SHARED) == -1){
            perror("Error occurred during shm_unlink for chairs ");
            exit(1);
        }
        if(shm_unlink(FIFO) == -1){
            perror("Error occurred during shm_unlink for chairs ");
            exit(1);
        }
        exit(0);
    }
}


int main(int args, char** argv) {
    if(args!=2){
        printf("Wrong number of arguments\n");
        exit(1);
    }
    if(( barber = sem_open(BARBER,O_CREAT ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,0)) ==SEM_FAILED){
        perror("Error during sem_open for barber");
        exit(1);
    }
    if((client = sem_open(CLIENT, O_CREAT ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,0) )== SEM_FAILED){
        perror("Error during sem_open for client");
        exit(1);
    }
    if(( mutex = sem_open(MUTEX,O_CREAT ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,1)) == SEM_FAILED){
        perror("Error during sem_open for mutex ");
        exit(1);
    }

    int shared_des = shm_open(SHARED,O_CREAT | O_RDWR,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if(shared_des ==-1){
        perror("Error during shm_open for waiting");
        exit(1);
    }
    struct timespec time;
    if( ftruncate(shared_des,sizeof(int))==-1){
        perror("Error during ftruncate for waiting");
        exit(1);
    }

    waiting = mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,shared_des,0);
    if(waiting==MAP_FAILED){
        perror("Error during mmap for waiting");
        exit(1);
    }
    *waiting =0;
    int chair_des = shm_open(CHAIRS,O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if(chair_des ==-1){
        perror("Error during shm_open for chairs");
        exit(1);
    }
   if( ftruncate(chair_des,sizeof(int))==-1){
       perror("Error during ftruncate for chairs");
       exit(1);
   }

    chairs = mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,chair_des,0);
    if(chairs==MAP_FAILED){
        perror("Error during mmap for chairs");
        exit(1);
    }
    init();
    *chairs= atoi(argv[1]);
    pid_t pid;
    int value;
    signal(SIGINT,hadler);
    printf("Barber opens shop\n");
    printf("%d chairs at the barber shop\n", *chairs);
    while (1){
       if( sem_wait(client)==-1){
           perror("Error during sem_wait(client)");
           exit(1);
       }
        if( sem_wait(mutex)==-1){
            perror("Error during sem_wait(mutex)");
            exit(1);
        }
        sleep(1);
        *waiting=*waiting-1;
        if(sem_post(barber)==-1){
            perror("Error during sem_pos(barber)");
            exit(1);
        }
        if(sem_post(mutex)==-1){
            perror("Error during sem_pos(mutex)");
            exit(1);
        }
        if(!isEmpty()) {
            pid= pop();
        }
        clock_gettime(CLOCK_MONOTONIC,&time);
        printf("The barber is now cutting hair %d  %ld\n",pid,(long)(time.tv_sec*1000000+time.tv_nsec));
        sleep(3);
        clock_gettime(CLOCK_MONOTONIC,&time);
        printf("Barber finished cutting hair of customer %d %ld \n",pid,(long)(time.tv_sec*1000000+time.tv_nsec));
        if(sem_getvalue(client,&value)==-1){
            perror("Error during sem_getvalue(client,&value)");
            exit(1);
        }
        clock_gettime(CLOCK_MONOTONIC,&time);
        if(value ==0) printf("Barber is sleeping %ld \n",(long)(time.tv_sec*1000000+time.tv_nsec));
    }


    return 0;
}