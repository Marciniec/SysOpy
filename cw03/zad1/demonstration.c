#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <memory.h>

int main(int argc, char ** argv){
    if (argc <2){
        perror("To few arguments");
        exit(1);
    }
    if(argc > 2){
        errno = 7;
        perror("There should be no more than 2 arguments");
        exit(1);
    }

    char * tmp;
    tmp = getenv(argv[1]);
    if(tmp ==NULL) printf("There's no environmental variable %s \n", argv[1]);
    else printf("%s=%s \n",argv[1],tmp);

    return 0;
}
