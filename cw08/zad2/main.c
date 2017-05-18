#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <memory.h>
#include <asm/errno.h>
#include <signal.h>

int file_des;
int record_number;
size_t buffer = 1024;
char *word;
int thread_no;
struct record{
    int id;
    char text[1020];
};
int finish = 0;
pthread_t *threads_arr;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2= PTHREAD_MUTEX_INITIALIZER;
long gettid() {
    return syscall(SYS_gettid);
}

void handler(int signum){
    printf("Received signal: %s, my PID is: %d, my TID is: %ld\n",strsignal(signum),getpid(), gettid());
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
//      sigset_t sigset;
//      sigfillset(&sigset);
//      pthread_sigmask(SIG_SETMASK,&sigset,NULL);
    signal(SIGUSR1,handler);
    signal(SIGTERM,handler);
    signal(SIGKILL,handler);
    signal(SIGSTOP,handler);
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < record_number; ++i) {

        if(finish) break;
        struct record * buf = malloc(sizeof(struct record));
        char * recordID = malloc((buffer+1)*sizeof(char));
        actually_read =  read(file_des, buf, buffer);
        if(actually_read==-1){
            perror("Error occurred during reading");
            exit(1);
        }
        if(actually_read==0) break;
        strcpy(recordID,buf->text);
        recordID[actually_read]='\0';
        if(strstr(recordID,word)!=NULL){
            printf("Thread with TID: %ld found %s in record %d\n",gettid(),word,buf->id);
            pthread_mutex_unlock(&mutex);
            free(recordID);
            pthread_cancel(pthread_self());
            return (void*)0;
        }
    }
    pthread_mutex_unlock(&mutex);

    return (void*)0;

}

void * bfunction(void * arg){
    int x = 17/0;
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
//    sigset_t sigset;
//    sigfillset(&sigset);
//    pthread_sigmask(SIG_SETMASK,&sigset,NULL);
//    raise(SIGUSR1);
//    raise(SIGTERM);
//    raise(SIGKILL);
//    raise(SIGSTOP);
//    pthread_kill(threads_arr[0],SIGUSR1);
//    pthread_kill(threads_arr[0],SIGTERM);
//    pthread_kill(threads_arr[0],SIGKILL);
    pthread_t divide;
    if(pthread_create(&divide,NULL, &bfunction,NULL)){
        perror("Error during thread creation");
        exit(1);
    }
    pthread_join(divide, NULL);
    //pthread_kill(threads_arr[0],SIGSTOP);

    for (int j = 0; j < thread_no; ++j) {
        pthread_join( threads_arr[j], NULL);

    }
    if(close(file_des)==-1){
        perror("Error during closing file");
        exit(1);
    }
    return 0;
}