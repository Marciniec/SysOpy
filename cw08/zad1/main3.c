#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <memory.h>
#include <asm/errno.h>

int file_des;
int record_number;
size_t buffer = 1024;
char *word;
int thread_no;
pthread_t *threads_arr;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2= PTHREAD_MUTEX_INITIALIZER;
struct record{
    int id;
    char text[1020];
};
int finish =0;
long gettid() {
    return syscall(SYS_gettid);
}

void *threadFunction(void *arg){

    if(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL)==EINVAL){
        fprintf(stderr,"Error occurred during pthread_setcancelstate\n");
        exit(1);
    }
    if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL)==EINVAL){
        fprintf(stderr,"Error occurred during pthread_setcanceltype\n");
        exit(1);
    }

    ssize_t actually_read;
    while (!finish) {
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < record_number; ++i) {


            struct record *buf = malloc(sizeof(struct record));
            char *recordID = malloc((buffer + 1) * sizeof(char));
            actually_read = read(file_des, buf, buffer);
            if (actually_read == -1) {
                perror("Error occurred during reading");
                exit(1);
            }
            strcpy(recordID, buf->text);
            recordID[actually_read] = '\0';
            if (strstr(recordID, word) != NULL) {
                printf("Thread with TID: %ld found %s in record %d\n", gettid(), word, buf->id);
            }
            if (actually_read == 0) {
                finish = 0;
                for (int j = 0; j < thread_no; ++j) {
                    if (!pthread_equal(threads_arr[j], pthread_self())) pthread_cancel(threads_arr[j]);
                }
                pthread_mutex_unlock(&mutex);
                free(recordID);
                pthread_cancel(pthread_self());
                return (void *) 0;
            }

        }
        pthread_mutex_unlock(&mutex);
    }
    return (void*)0;

}

int main(int args, char**argv) {
    if(args!=5){
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }
    thread_no=atoi(argv[1]);
    file_des= open(argv[2] ,O_RDONLY);
    if (file_des==-1){
        perror("Error during opening file");
        exit(1);
    }
    record_number = atoi(argv[3]);
    word=argv[4];
    threads_arr =  malloc(sizeof(pthread_t) * thread_no);

    pthread_mutex_lock(&mutex2);
    for (int i = 0; i < thread_no; ++i) {
        if(pthread_create(&threads_arr[i],NULL, &threadFunction,NULL)){
            perror("Error during thread creation");
            exit(1);
        }
    }
    pthread_mutex_unlock(&mutex2);
    for (int j = 0; j < thread_no; ++j) {
        pthread_join( threads_arr[j], NULL);

    }
    if(close(file_des)==-1){
        perror("Error during closing file");
        exit(1);
    }
    return 0;
}