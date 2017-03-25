#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void blockReadChar(int file, int place, int cmd){
    struct flock *flockptr = malloc(sizeof(struct flock));
    flockptr ->l_whence = SEEK_SET;
    flockptr ->l_start = place;
    flockptr -> l_type = F_RDLCK;
    flockptr->l_len = 1;
    if( fcntl(file,F_GETLK,flockptr) ==-1){
        perror("fcntl error");
        exit(1);
    }
    free(flockptr);
}
void blockWriteChar(int file, int place, int cmd){
    struct flock * flockptr = malloc(sizeof(struct flock));
    flockptr->l_whence= SEEK_SET;
    flockptr ->l_start = place;
    flockptr->l_type = F_WRLCK;
    flockptr->l_len = 1;
    if( fcntl(file,F_GETLK,flockptr) ==-1){
        perror("fcntl error");
        exit(1);
    }
    free(flockptr);
}
void showBlocks(int file){
    long int fileBegining = lseek(file,0,SEEK_SET);
    long int fileEnd = lseek(file,0,SEEK_END);
    if (file<0){
        perror("");
        exit(1);
    }
    while (fileBegining != fileEnd){
        struct flock *flockRptr = malloc(sizeof(struct flock));
        flockRptr ->l_whence = SEEK_SET;
        flockRptr ->l_start = fileBegining;
        flockRptr -> l_type = F_RDLCK;
        flockRptr->l_len = 1;
        struct flock * flockWptr = malloc(sizeof(struct flock));
        flockWptr->l_whence= SEEK_SET;
        flockWptr ->l_start = fileBegining;
        flockWptr->l_type = F_WRLCK;
        flockWptr->l_len = 1;
        if( fcntl(file,F_GETLK,flockRptr) ==-1){
            perror("fcntl error");
            exit(1);
        }
        if (flockRptr->l_type!=F_UNLCK) printf("Offset: %ld PID: %d read block \n", fileBegining,flockRptr->l_pid);
        if( fcntl(file,F_GETLK,flockWptr) ==-1){
            perror("fcntl error");
            exit(1);
        }
        if (flockWptr->l_type!=F_UNLCK) printf("Offset: %ld PID: %d  write block \n", fileBegining, flockWptr->l_pid);

        fileBegining++;
    }
}
void unlockChar(int file, int place){
    struct flock * flockptr = malloc(sizeof(struct flock));
    flockptr ->l_type = F_UNLCK;
    flockptr->l_whence = SEEK_SET;
    flockptr ->l_start = place;
    fcntl(file,F_SETLK,flockptr);
}
void readChar(int file, int place){
    lseek(file,place,SEEK_SET);
    char * buff  =malloc(sizeof(char));
    read(file,buff,sizeof(char));
    printf("%s",buff);
}
void writeChar(int file, int place){
    lseek(file,place,SEEK_SET);
    char buff[2];
    printf("Insert character: ");
    scanf("%s",buff);
    write(file,buff,sizeof(char));
}

void printOptions(){
    printf("r -offset, read lock on one byte of file with offset - not blocking \n" );
    printf("b -offset, read lock on one byte of file with offset -  blocking \n" );
    printf("w -offset, write lock on one byte of file with offset - not blocking \n" );
    printf("d -offset, write lock on one byte of file with offset -  blocking \n" );
    printf("a shows all locks \n" );
    printf("u -offset, unlock one byte of file with offset \n" );
    printf("f -offset, reads one char of file with offset \n" );
    printf("h -offset, writes one char of file with offset \n" );
    printf("q - quit \n");

}


int main(int argc, char ** argv) {
    int end =0;
    int place;
    if(argc <2){
        errno = 22;
        perror("To few arguments");
        exit(1);
    }
    if(argc >2){
        errno = 7;
        perror("There should be no more than 3 arguments");
        exit(1);
    }
    char * filePath = argv[1];
    int file = open(filePath,O_RDWR);
    if (file<0){
        perror(filePath);
        exit(1);
    }
    char *options = malloc(1) ;

    printOptions();
    while (!end){
        scanf("%s", options);
        switch (options[0]) {
            case 'r':  {
                printf("Insert offset: ");
                scanf("%d", &place);
                blockReadChar(file, place, F_SETLK);
            }
                break;
            case 'b': {
                printf("Insert offset: ");
                scanf("%d", &place);
                blockReadChar(file, place, F_SETLKW);
            }
                break;
            case 'w': {
                printf("Insert offset: ");
                scanf("%d", &place);
                blockWriteChar(file, place, F_SETLK);
            }
                break;
            case 'a': {
                showBlocks(file);
            }
                break;
            case'd': {
                printf("Insert offset: ");
                scanf("%d", &place);
                blockWriteChar(file, place, F_SETLKW);
            }
                break;
            case  'u': {
                printf("Insert offset: ");
                scanf("%d", &place);
                unlockChar(file, place);
            }
                break;
            case 'f': {
                printf("Insert offset: ");
                scanf("%d", &place);
                readChar(file, place);
            }
                break;
            case  'h': {
                printf("Insert offset: ");
                scanf("%d", &place);
                writeChar(file, place);
            }
                break;
            case 'q' :  {
                end = 1;
                close(file);
            }
                break;
            default:
                printf("Invalid option \n");
                printOptions();
                break;
        }
    }

    return 0;
}