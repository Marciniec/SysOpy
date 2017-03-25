#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ftw.h>
#include <stdint.h>
#include <errno.h>

int size;
char* formatdate(time_t val)
{   char * date = malloc(12);
    strftime(date, 36, "%d.%m.%Y %H:%M:%S", localtime(&val));
    return date;
}
char * fileaccess(const struct stat *buff){
    char * access = malloc(9);
    if (buff->st_mode & S_IRWXU) {
        access[0] ='r';
        access[1] ='w';
        access[2] ='x';
    } else {
        access[0] = (char) ((buff->st_mode & S_IRUSR) ? 'r' : '-');
        access[1] = (char) ((buff->st_mode & S_IWUSR) ? 'w' : '-');
        access[2] = (char) ((buff->st_mode & S_IXUSR) ? 'x' : '-');
    }
    if (buff->st_mode==S_IRWXG) {
        access[3] ='r';
        access[4] ='w';
        access[5] ='x';
    } else {
        access[3] = (char) ((buff->st_mode & S_IRGRP) ? 'r' : '-');
        access[4] = (char) ((buff->st_mode & S_IWGRP) ? 'w' : '-');
        access[5] = (char) ((buff->st_mode & S_IXGRP) ? 'x' : '-');
    }
    if (buff->st_mode==S_IRWXO) {
        access[6] ='r';
        access[7] ='w';
        access[8] ='x';
    } else {
        access[6] = (char) ((buff->st_mode & S_IROTH) ? 'r' : '-');
        access[7] = (char) ((buff->st_mode & S_IWOTH) ? 'w' : '-');
        access[8] = (char) ((buff->st_mode & S_IXOTH) ? 'x' : '-');
    }
    access[9]='\0';
    return access;
}

int showParameters(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
    char * access;
    char * date;
    if(tflag == FTW_F && sb->st_size <size){
        printf("File path:  %s \n", fpath);
        printf("File size: %dB\n",(int) sb->st_size);
        access = fileaccess(sb);
        printf("File accces: %9s \n", access);
        date = formatdate(sb->st_mtime);
        printf("Last modified: %s \n",date);

    }
    return 0;
}

int main(int argc, char**argv){
   
    if(argc <3){
        errno = 22;
        perror("To few arguments");
        exit(1);
    }
    if(argc >3){
        errno = 7;
        perror("There should be no more than 3 arguments");
        exit(1);
    }
	char * path;
    path = argv[1];
    size = atoi(argv[2]);
    if(nftw(path,showParameters,100,1) == -1){
        printf("nftw fun error\n");
        return 1;
    }

    return 0;
}


