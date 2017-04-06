#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int up;
void handle_tstp(int signum){
   if(signum==SIGTSTP) up = -up;
    else if (signum ==SIGINT) {
       write(1, "Recived signal SINGINT\n", 23);
        exit(0);
   }
}

int main() {
    struct sigaction new_action, old_action;
    new_action.sa_handler = handle_tstp;
    __sighandler_t handler = handle_tstp;
    char*  alphabet[26] ={"A\n","B\n","C\n","D\n","E\n","F\n","G\n","H\n","I\n","J\n", "K\n", "L\n"
            , "M\n", "N\n", "O\n","P\n","Q\n","R\n","S\n","T\n","U\n","V\n","W\n","X\n","Y\n","Z\n" };
    int i = 0;
    up = -1;
    while (1){
        write(1,alphabet[i],sizeof(char)*2);
        if(up==-1){
            i=(i+1)%26;
        } else if(up==1){
            if(i==0) i = 25;
            else i=i-1;
            }
        sigaction(SIGTSTP,&new_action, &old_action);
        signal(SIGINT,handler);
        sleep(2);
    }

    return 0;
}