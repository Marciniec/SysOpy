#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <memory.h>
#include <wait.h>

extern char ** environ;

void handleEnviromentVariable(char* line){
    char * envName;
    char *value;
    char * token;
    token = strtok(line, " ");
    envName = token+1;
    token = strtok(NULL, " ");
    value = token;
    if(value == NULL) {
      for (int i = 0; envName[i]!='\0'; i++) {
        if(envName[i]=='\n') envName[i] = '\0';
      }
      unsetenv(envName);
    }
    else setenv(envName,value, 1);

}
void handleFunctions(char *line){
    char * args[10]={NULL};
    char * tmp=malloc(sizeof(line));
    sprintf(tmp,"%s", line);
    int i = 0;
    char * token;
    int pid;
    int waitstatus;
    token = strtok(tmp, " ");
    args[i] = token;
    i++;
    while (token !=NULL){
        token = strtok(NULL, " ");
        args[i] = token;
        i++;
    }
    i-=2;
    for (int j = 0; args[i][j]!='\0'; ++j) {  //changing '\n' into '\0'
        if( args[i][j]=='\n') args[i][j]='\0';
    }

    pid = fork();
    if(pid == 0) {
      if(execvp(args[0], args) == -1){
          perror(args[0]);
          exit(1);
      }
    } else if (pid > 0){
          wait(&waitstatus);
          if(WIFEXITED(waitstatus) &&WEXITSTATUS(waitstatus)!=0){
              perror("Child process failure");
              exit(2);
          }
    } else {
          perror(args[0]);
          exit(3);
      }
    }

int main(int argc, char ** argv) {
    if (argc <2){
        perror("To few arguments");
        exit(1);
    }
    if(argc > 2){
        errno = 7;
        perror("There should be no more than 2 arguments");
        exit(1);
    }
    FILE * file = fopen(argv[1],"r");
    char *lineptr=NULL;
    size_t n ;
    size_t read;
    while ((read = (size_t) (getline(&lineptr, &n, file) != -1))){
        if(lineptr[0]=='#'){
            handleEnviromentVariable(lineptr);
        } else{
            handleFunctions(lineptr);
        }
    }

    return 0;
}
