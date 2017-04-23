#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <wait.h>

void parse_command(char *command){
    char ** commands = malloc(256);
    char * token;
    char * tmp = malloc(256);
    char * saveptr;
    sprintf(tmp, "%s", command);
    int no_commands = 0;
    int i =  0;
    token = strtok_r(tmp, "|",&saveptr);
    commands[i] = token;
    i++;
    while (token !=NULL){
        token = strtok_r(NULL, "|", &saveptr);
        commands[i] = token;
        i++;
    }
    no_commands=i-1;
    commands[no_commands-1][strlen(commands[no_commands-1])-1] = '\0';  // changing \'n' to \'0'
    char ** args = malloc(256);
    int in =STDIN_FILENO;
    int fd[2];

    for (int j = 0; j < no_commands; ++j) {
        sprintf(tmp,"%s",commands[j]);
        token = strtok_r(tmp," ",&saveptr);
        args[0] = token;
        int k = 1;
        while (token !=NULL){
            token = strtok_r(NULL, " ",&saveptr);
            args[k] = token;
            k++;
        }
        pipe(fd);
        if (j + 1 == no_commands){
            int pid = fork();
            if (pid==0){
                if(in != -1 && in != STDIN_FILENO) dup2(in,STDIN_FILENO);
                if(execvp(args[0],args) == -1){
                    perror(args[0]);
                    exit(1);
                }
            } else {
                close(fd[1]);
                if(fd[0]!=STDIN_FILENO){
                    dup2(fd[0],STDIN_FILENO);
                }
                wait(NULL);
                if(in != -1) close(in);
            }
        } else{
            int pid = fork();
            if (pid==0){
                if(in != -1 && in != STDIN_FILENO) dup2(in,STDIN_FILENO);
                dup2(fd[1],STDOUT_FILENO);
                if(execvp(args[0],args) == -1){
                    perror(args[0]);
                    exit(1);
                }
            } else {
                close(fd[1]);
                if(fd[0]!=STDIN_FILENO){
                    dup2(fd[0],STDIN_FILENO);
                }
                wait(NULL);
                if(in != -1) close(in);
            }
        }
        in = fd[0];
        close(fd[1]);

    }
    close(fd[0]);
    free(args);

}


int main() {
    size_t len = 0;
    char * line=NULL;
    size_t nread;
    char ** cc = malloc(250);
    for (int k = 0; k < 250; ++k) {
        cc[k] = malloc(250);
    }
    int i =0;
    while ((nread = getline(&line, &len, stdin)) != -1 && line[0] != '\n') {
        parse_command(line);
    }
    return 0;
}