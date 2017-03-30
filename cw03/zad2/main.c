#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <memory.h>
#include <wait.h>
#include <sys/resource.h>

void handleEnviromentVariable(char* line){
    char * envName;
    char *value;
    char * token;
    line[strlen(line) -1] = '\0';  //changing '\n' to '\0'
    token = strtok(line, " ");
    envName = token+1;
    token = strtok(NULL, " ");
    value = token;
    if(value == NULL) {
        unsetenv(envName);
    }
    else setenv(envName,value, 1);

}
void handleFunctions(char *line, int CPUlimit, int VSZlimit){
    char * args[10]={NULL};
    char * tmp=malloc(sizeof(line));
    line[strlen(line) -1] = '\0';  //changing '\n' to '\0'
    sprintf(tmp,"%s", line);
    int i = 0;
    char * token;
    int pid;
    int waitstatus;
    struct rlimit CPUlim;
    struct rlimit VSZlim;
    CPUlim.rlim_max = (rlim_t) CPUlimit;
    CPUlim.rlim_cur= (rlim_t) CPUlimit -1;
    VSZlim.rlim_max= (rlim_t) VSZlimit* 1000000; //B to MB
    VSZlim.rlim_cur= (rlim_t) VSZlimit* 1000000 -1;
    struct rusage usage;
    token = strtok(tmp, " ");
    args[i] = token;
    i++;
    while (token !=NULL){
        token = strtok(NULL, " ");
        args[i] = token;
        i++;
    }
    pid = fork();
    if(pid == 0) {
        setrlimit(RLIMIT_CPU,&CPUlim);
        setrlimit(RLIMIT_AS,&VSZlim);
        if(execvp(args[0], args) == -1){
            perror(args[0]);
            exit(1);
        }
    } else if (pid > 0){
        wait3(&waitstatus,0,&usage);
        if(WIFEXITED(waitstatus) &&WEXITSTATUS(waitstatus)!=0){
            perror("Child process failure");
            exit(2);
        } else{
            printf("Resources  user time : %012ld ms system time : %012ld ms \n",usage.ru_utime.tv_sec*1000000+usage.ru_utime.tv_usec, usage.ru_stime.tv_sec*1000000+usage.ru_stime.tv_usec);
        }
    } else {
        perror(args[0]);
        exit(3);
    }
}

int main(int argc, char ** argv) {
    if (argc <4){
        perror("To few arguments");
        exit(1);
    }
    if(argc >4){
        errno = 7;
        perror("There should be no more than 3 arguments");
        exit(1);
    }
    int cpu = atoi(argv[2]);
    int vsz = atoi(argv[3]);
    FILE * file = fopen(argv[1],"r");
    char *lineptr=NULL;
    size_t n ;
    size_t read;
    while ((read = (size_t) (getline(&lineptr, &n, file) != -1))){
        if(lineptr[0]=='#'){
            handleEnviromentVariable(lineptr);
        } else{
            handleFunctions(lineptr,cpu,vsz);
        }
    }
    fclose(file);
    return 0;
}
