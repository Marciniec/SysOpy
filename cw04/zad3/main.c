#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

volatile sig_atomic_t  parent_received_signals;
volatile sig_atomic_t  parent_send_signals;
volatile sig_atomic_t  child_received_signals;
int PID, PPID;
int L, type;

void typeOne(){
    for (int i = 0; i < L; ++i) {
        kill(PID,SIGUSR1);
        usleep(1500);
        parent_send_signals++;
    }
    kill(PID,SIGUSR2);
    parent_send_signals++;
}
void typeTwo(){
    union sigval value;
    for (int i = 0; i < L; ++i) {
        sigqueue(PID,SIGUSR1,value);
        usleep(1500);
        parent_send_signals++;
    }
    usleep(1500);
    sigqueue(PID,SIGUSR2,value);
    parent_send_signals++;
}
void typeThree(){
    for (int i = 0; i < L; ++i) {
        kill(PID,SIGRTMIN +7);
        usleep(1500);
        parent_send_signals++;
    }
    kill(PID,SIGRTMIN +17);
    parent_send_signals++;
}


void child_handler(int signum){
    char * message = malloc(35);
    if(signum == SIGUSR1 ){
        child_received_signals++;
        kill(PPID,SIGUSR1);
    } else if (signum == SIGUSR2){
        child_received_signals++;
        sprintf(message,"Child %d received %d signals from parent: %d",getpid(),child_received_signals,PPID);
        write(1,message,50);
        exit(0);
    } else if(signum == SIGRTMIN +7 ){
        child_received_signals++;
        kill(PPID,SIGRTMIN +7);
    } else if (signum == SIGRTMIN +17){
        child_received_signals++;
        sprintf(message,"Child %d, received %d signals from parent: %d",getpid(),child_received_signals,PPID);
        write(1,message,50);
        exit(0);
    }

}

void parent_handler(int signum) {
    if (signum == SIGUSR1) {
        parent_received_signals++;
    } else if (signum == SIGRTMIN +7){
        parent_received_signals++;
    }
}

void set_child(){
    signal(SIGUSR1,child_handler);
    signal(SIGUSR2,child_handler);
    signal(SIGRTMIN+7,child_handler);
    signal(SIGRTMIN+17,child_handler);
    while (1);
}

int main(int argc, char ** argv) {
    if(argc!=3){
        perror("There should be exactly 2 arguments");
        exit(1);
    }
    L = atoi((const char *) argv[1]);
    type = atoi((const char *) argv[2]);
    signal(SIGUSR1,parent_handler);
    PPID = getpid();
    PID = fork();
    if (PID == 0){
       set_child();

    } else if (PID>0){
        switch (type) {
            case 1:
                typeOne();
                break;
            case 2:
                typeTwo();
                break;
            case 3:
                typeThree();
                break;
            default:break;
        }
    }
    waitpid(PID,NULL,0);
    printf("\n Signals send by parent %d\n",parent_send_signals);
    printf("\n Signals received by parent %d\n", parent_received_signals);


    return 0;
}