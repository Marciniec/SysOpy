#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <bits/siginfo.h>


pid_t  * waiting_children;
struct timespec startRealTime, endRealTime;
int signalsRecived;
int k;

void sleep_and_send_signal_to_parent(pid_t parentpid){
    sleep(2);
    char * mess = malloc(50);
    write(1,mess, 50);
    clock_gettime(CLOCK_MONOTONIC_RAW, &startRealTime);
    kill(parentpid,SIGRTMIN);
}
void  send_signal_to_child(pid_t cpid){
    kill(cpid,SIGUSR2);
}
void handleParentAction(int signum, siginfo_t *signalinfo){
    if(signum == SIGRTMIN){
        char * message = malloc(45);
        waiting_children[signalsRecived] =signalinfo->si_pid;
        signalsRecived++;
        sprintf(message,"%d received %d. SIGRTMIN form child %d\n", getpid(), signalsRecived,(int) signalinfo->si_pid);
        write(1,message,45);
        if (signalsRecived==k){
            for (int i = 0; i < k; ++i) {
                send_signal_to_child(waiting_children[i]);
            }
        }
        if(signalsRecived>k) send_signal_to_child(signalinfo->si_pid);
    } else{
        char * message = malloc(50);
        sprintf(message, "Received Real Time signal: %d from child %d\n",  signalinfo->si_signo, (int) signalinfo->si_pid);
        write(1,message,50);
    }
}
void handleChildAction(int signum, siginfo_t *signalinfo){
    if (signum == SIGUSR2){
        char * message = malloc(50);
        sprintf(message, "Child %d received signal SIGUSR2 from %d\n",getpid(), signalinfo->si_signo);
        write(1,message,50);
        char * time = malloc(45);
        clock_gettime(CLOCK_MONOTONIC_RAW, &endRealTime);
        long rt = endRealTime.tv_sec*1000000 +endRealTime.tv_nsec -(startRealTime.tv_sec*1000000+startRealTime.tv_nsec);
        sprintf(time, "Real time of child %d is %6ld ms \n", getpid(), rt);
        write(1,time,45);
        kill(signalinfo->si_pid,SIGRTMIN+rand()%32);
        exit(0);
    }
}
int main(int argc, char ** argv) {
    pid_t PPID = getpid();
    srand((unsigned int) time(NULL));
    signalsRecived = 0;
    if(argc < 3){
        perror("There should be exactly 2 arguments");
        exit(1);
    } else if (argc > 3){
        perror("There should be exactly 2 arguments");
        exit(1);
    }
    int n = atoi(argv[1]);
    k = atoi(argv[2]);
    pid_t PID;
    waiting_children = malloc(sizeof(pid_t)*k);
    struct sigaction parentAction, childAction;
    parentAction.sa_handler = (__sighandler_t) handleParentAction;
    childAction.sa_handler = (__sighandler_t) handleChildAction;
    childAction.sa_flags =SA_SIGINFO;
    parentAction.sa_flags = SA_SIGINFO;
    for (int i = 0; i < n; ++i) {
        PID = fork();
        if(PID == 0){
            srand((unsigned int) (time(NULL) + getpid()));
            sleep_and_send_signal_to_parent(PPID);
            sigaction(SIGUSR2,&childAction,NULL);
            pause();
        } else if(PID >0){
            sigaction(SIGUSR1,&parentAction,NULL);
            for (int j = SIGRTMIN; j < SIGRTMAX; ++j) {
                sigaction(j,&parentAction,NULL);
            }
           pause();
        }

    }
//    wait(&waiting_children[9]);
//    for (int j = 0; j < n; ++j) {
//        kill(waiting_children[j],SIGKILL);
//    }
  while (1){
        ;
    }
    return 0;
}