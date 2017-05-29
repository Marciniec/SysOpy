#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include "readerswriters.h"
#define NOREADERS 20
#define NOWRITERS 10
int array [100];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mFinish = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cFinish = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t fifoMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t  * permsons;
int writ_amount, novelistCheck, read_amount, anyReaders;
int exitF;
struct FIFO *fifo;
void init(){
    fifo = malloc(sizeof(struct FIFO));
    fifo->head = NULL;

}
void push(int ID, human h){
    struct node * element = malloc(sizeof(struct node));
    element ->next = NULL;
    element -> ID = ID;
    element -> h = h;
    if(fifo->head == NULL){
        fifo->head = element;
    } else fifo->tail->next = element;

    fifo->tail = element;
}
struct node pop (){
    struct node result;
    if(fifo ->head ==NULL){
        result.ID = -1 ;
        return result;
    }
    result = *(fifo->head);
    struct node *tmp = fifo->head;
    fifo->head = fifo->head->next;
    free(tmp);
    return result;
}
void readF(int n){
    int counter = 0;
    for (int i = 0; i < 100; ++i) {
        if(array[i] %n ==0){
            counter++;
            printf("%d on index %d can be divided by %d \n",array[i], i, n);
        }
    }
    printf("Found %d numbers divisible by %d\n", counter,n);
}
void writeF(){
    int rand_number;
    int index;
    int value;
    rand_number = rand() % 100;
    for (int i = 0; i < rand_number; ++i) {
            index = rand() % 100;
            value = rand();
            array[index] = value;
        printf("Writer wrote %d into array on index: %d\n",value,index);
    }

    printf("Writer wrote total %d numbers. \n",rand_number);
}

void clean(struct FIFO * fifo){
    struct node * tmp;
    while (fifo ->head!=NULL){
        tmp = fifo->head;
        fifo->head = fifo->head->next;
        free(tmp);
    }
}

void * writer_function(void*arg){
    struct writer * writer = (struct writer *) arg;
    struct node tmp;
    tmp.ID = writer ->ID;
    tmp.h = (human) writer;
    while (1) {
        pthread_mutex_lock(&fifoMutex);
        push(tmp.ID,tmp.h);
        pthread_mutex_unlock(&fifoMutex);
        pthread_mutex_lock(&mutex);
        pthread_mutex_lock(&mutex2);
        writ_amount++;
        pthread_cond_signal(&condition2);
        pthread_mutex_unlock(&mutex2);
        while (permsons[tmp.ID]==0){
            pthread_cond_wait(&condition,&mutex);
        }
        pthread_mutex_unlock(&mutex);
        writeF();
        pthread_mutex_lock(&mFinish);
        if(tmp.h == (human) writer) novelistCheck = 0;
        else {
            --read_amount;
            if(read_amount ==0) anyReaders=0;
        }
        permsons[tmp.ID] = 0;
        pthread_cond_signal(&cFinish);
        pthread_mutex_unlock(&mFinish);
    }

}
void * reader_function(void *arg){
    struct reader * reader = (struct reader *) arg;
    struct node tmp;
    tmp.h = (human) reader;
    tmp.ID = reader ->ID;
    while (1){
        pthread_mutex_lock(&fifoMutex);
        push(tmp.ID,tmp.h);
        pthread_mutex_unlock(&fifoMutex);
        pthread_mutex_lock(&mutex);
        pthread_mutex_lock(&mutex2);
        writ_amount++;
        pthread_cond_signal(&condition2);
        pthread_mutex_unlock(&mutex2);
        while (permsons[tmp.ID]==0){
            pthread_cond_wait(&condition,&mutex);
        }
        pthread_mutex_unlock(&mutex);
        readF(reader->divider);
        pthread_mutex_lock(&mFinish);
        if(tmp.h == (human) writer) novelistCheck = 0;
        else {
            --read_amount;
            if(read_amount ==0) anyReaders=0;
        }
        permsons[tmp.ID] = 0;
        pthread_cond_signal(&cFinish);
        pthread_mutex_unlock(&mFinish);
    }

}
void exit_handler(){
    clean(fifo);
}
void sighandler(int singnum){
    printf("SIGINT\n");
    exit_handler();
    exit(0);
}

int main(int argc, char ** argv) {
    if(argc!=2){
        fprintf(stderr,"Wrong number of arguments");
        exit(1);
    }
    permsons = malloc(sizeof(pthread_t)* (NOWRITERS+NOREADERS));
    init();
    signal(SIGINT,sighandler);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_t  * threadID = malloc((NOREADERS+NOWRITERS)*sizeof(pthread_t));
    struct reader ** readers_array = malloc(sizeof(struct reader *)* NOREADERS);
    struct writer ** writers_array = malloc(sizeof(struct writer *)* NOWRITERS);
    srand((unsigned int) time(NULL));
    for (int i =0; i <100; i++){
        array[i] = rand();
    }
    for (int i = 0; i < NOREADERS; ++i) {
        readers_array[i] = malloc(sizeof(struct reader));
        readers_array[i] ->divider = atoi(argv[1]);
        readers_array[i] ->ID = i;
        pthread_create(&threadID[i], &attr,reader_function,readers_array[i]);
    }
    int j;
    for (j = 0; j < NOWRITERS; ++j) {
        writers_array[j] = malloc(sizeof(struct writer));
        writers_array[j]->ID = NOREADERS + j;
        pthread_create(&threadID[NOREADERS+j],&attr,writer_function,writers_array[j]);
    }
    struct node tmp;
    while (exitF ==0){
        pthread_mutex_lock(&mutex2);
        while (writ_amount==0){
            pthread_cond_wait(&condition2,&mutex2);

        }
        writ_amount--;
        pthread_mutex_unlock(&mutex2);
        pthread_mutex_lock(&fifoMutex);
        tmp = pop();
        pthread_mutex_unlock(&fifoMutex);
        if(tmp.h == writer){
            pthread_mutex_lock(&mFinish);
            while (anyReaders == 1 || novelistCheck ==1){
                pthread_cond_wait(&cFinish, &mFinish);
            }
            pthread_mutex_unlock(&mFinish);
            pthread_mutex_lock(&mutex);
            permsons[tmp.ID] = 1;
            pthread_cond_broadcast(&condition);
            pthread_mutex_unlock(&mutex);
        } else{
            pthread_mutex_lock(&mFinish);
            while (novelistCheck == 1) pthread_cond_wait(&cFinish, &mFinish);
            read_amount++;
            anyReaders = 1;
            pthread_mutex_unlock(&mFinish);
            pthread_mutex_lock(&mutex);
            permsons[tmp.ID] = 1;
            pthread_cond_broadcast(&condition);
            pthread_mutex_unlock(&mutex);
        }

    }
    while(j < NOWRITERS+NOREADERS){
        pthread_cancel(threadID[j]);
        j++;
    }
    j = 0;
    free(threadID);
    int iter1 =0, iter2 =0;
    while(iter1 < NOWRITERS){
        free(writers_array[iter1]);
        iter1++;
    }
    while(iter2 < NOREADERS){
        free(readers_array[iter2]);
        iter2++;
    }
    free(writers_array);
    free(readers_array);

    return 0;
}
