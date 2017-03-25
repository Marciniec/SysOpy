#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

char* fullpath(char * directoryPath, char * fileName ){
    char *newFile=malloc(256*sizeof(char));
    sprintf(newFile,"%s/%s%c",directoryPath,fileName,'\0');
    return newFile;
}

char * fileaccess(struct stat *buff){
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
char* formatdate(time_t val)
{   char * date = malloc(12);
    strftime(date, 36, "%d.%m.%Y %H:%M:%S", localtime(&val));
    return date;
}

void searchDirectory(char * path, int size){
    DIR *directory = opendir(path);
    if(directory == NULL){
        perror(path);
        exit(1);
    }
    char * wholepath = malloc(256);
    char * path1;
    char * access;
    char  * date;
    struct dirent *dirp;

    struct stat * buff = malloc(sizeof(stat));
    while ((dirp = readdir(directory))!=NULL){
        path1=fullpath(path,dirp->d_name);
        sprintf(wholepath,"%s",path1);   //coppying path due to change in lstat function
        if( lstat(path1,buff)<0){
            perror(path1);
            exit(1);
        }

        if (S_ISDIR(buff->st_mode) && !(S_ISLNK(buff->st_mode)) && dirp->d_name[0]!='.') searchDirectory(wholepath,size);
        else if (S_ISREG(buff->st_mode) && buff->st_size < size) {
           printf("File path:  %s \n", wholepath);
           printf("File size: %dB\n",(int) buff->st_size);
           access = fileaccess(buff);
           printf("File accces: %9s \n", access);
           date = formatdate(buff->st_mtim.tv_sec);
           printf("Last modified: %s \n",date);
        }

    }
}


int main(int argc, char **argv ){
    char * path;
    int size;
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
    path = argv[1];
    size = atoi(argv[2]);
    searchDirectory(path,size);


    return 0;
}

