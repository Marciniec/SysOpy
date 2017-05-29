#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define NOREADERS 20
#define  NOWRITERS 10

int array[100];
int opt;
pthread_t * readers_array;
pthread_t * writers_array;
sem_t semaphore;
int divider;

void sig_handler(int signum){
    if(signum == SIGINT){
        printf("SIGINT \n");
        exit(1);
    }
}

void * reader_function(void * arg){

    int how_many;
    for (int i = 0; i < NOREADERS; ++i) {
        sem_wait(&semaphore);
        how_many = 0;
        printf("Reader Started reading\n");
        for (int j = 0; j < 100; ++j) {
            if(array[j]%divider ==0){
                how_many++;
                if(opt){
                    printf("Number : %d in index : %d \n",array[j], j);
                }
            }
        }
        printf("Found %d divisible  by %d numbers\n",how_many, divider);

        sem_post(&semaphore);
        sleep(1);
    }
    return (void *) 0;
}

void  * writer_function(void * arg){
    int rand_number, index, rand_val;
    for (int i = 0; i < NOWRITERS; ++i) {
        sem_wait(&semaphore);
        printf("Writer starting to change numbers \n");
        rand_number = rand()%100;
        for (int j = 0; j < rand_number; ++j) {
            index = rand() % 100;
            rand_val = rand() % 100;
            if(opt){
                printf("Writer changed the value in index %d to %d \n",index,rand_val);
            }
            array[index] = rand_val;
        }
        sem_post(&semaphore);
        sleep(1);
    }
    return (void *) 0;
}

int main(int args, char ** argv) {
    if(args!=2){
        fprintf(stderr, "Wrong number of arguments");
        exit(1);
    }
    opt = 1;
    srand((unsigned int) time(NULL));
    readers_array = malloc(NOREADERS*sizeof(pthread_t));
    writers_array = malloc(NOWRITERS*sizeof(pthread_t));
    divider = atoi(argv[1]);
    signal(SIGINT, sig_handler);
    for (int i = 0; i < 100; ++i) {
        array[i] = rand();
    }
    if( sem_init(&semaphore, 0,1) == -1 ){
        perror("Error during sem_init");
        exit(1);
    }
    struct sched_param reader_param;
    reader_param.__sched_priority = 1;
    pthread_attr_t * attr_reader = malloc(sizeof(pthread_attr_t));
    pthread_attr_setschedpolicy(attr_reader,SCHED_FIFO);
    pthread_attr_setschedparam(attr_reader, &reader_param);
    for (int i = 0; i < NOREADERS; ++i) {
        if(pthread_create(&readers_array[i], attr_reader, reader_function,NULL) <0){
            perror("Error during pthread_create for writers");
            exit(1);
        }
    }
    struct sched_param writer_param;
    writer_param.__sched_priority = 0;
    pthread_attr_t * attr_writer = malloc(sizeof(pthread_attr_t));
    pthread_attr_setschedpolicy(attr_writer, SCHED_FIFO);
    pthread_attr_setschedparam(attr_writer, &writer_param);

    for (int i = 0; i < NOWRITERS; ++i) {
        if(pthread_create(&writers_array[i], attr_writer, writer_function,NULL) <0){
            perror("Error during pthread_create for writers");
            exit(1);
        }
    }

    for (int i = 0; i < NOREADERS; ++i) {
        if(pthread_join(readers_array[i],NULL)<0){
            perror("Error during pthread_join for writers");
            exit(1);
        }
    }
    for (int i = 0; i < NOWRITERS; ++i) {
        if(pthread_join(writers_array[i],NULL)<0){
            perror("Error during pthread_join for writers");
            exit(1);
        }
    }
    if(sem_destroy(&semaphore)==-1){
        perror("Error during deleting semaphore");
        exit(1);
    }
    free(readers_array);
    free(writers_array);
    return 0;
}