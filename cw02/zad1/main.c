#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <time.h>
#include <sys/resource.h>
#include <errno.h>

struct timespec startRealTime, endRealTime;
struct rusage endSysUsr,startSysUsr;
void printTime(){
    printf("User time: %010ld ms, System Time: %010ld ms, Real Time: %010ld\n",
           (long) ( (endSysUsr.ru_utime.tv_usec - startSysUsr.ru_utime.tv_usec)),
           (long) ((endSysUsr.ru_stime.tv_usec - startSysUsr.ru_stime.tv_usec)),
           (endRealTime.tv_sec - startRealTime.tv_sec) * 1000000000 + (endRealTime.tv_nsec - startRealTime.tv_nsec)
    );
}
void beginMeasurement(){
    getrusage(RUSAGE_SELF, &startSysUsr);
    clock_gettime(CLOCK_MONOTONIC_RAW, &startRealTime);

}
void finishMeasurement(){
    getrusage(RUSAGE_SELF, &endSysUsr);
    clock_gettime(CLOCK_MONOTONIC_RAW, &endRealTime);
}


void generateFile(char * fileName, int noRecords, int recordSize){
    int urandomFile = open("/dev/urandom",O_RDONLY);
    if (urandomFile <0) {
        perror("Error during opening dev/urandom file");
        exit(1);
    }
    int newFile = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH |S_IWUSR | S_IWGRP | S_IWOTH);
    if (newFile <0){
        perror("Error during creating new file");
        exit(1);
    }
    char * buffer = malloc(sizeof(char)*recordSize);
    for (int i = 0; i < noRecords; ++i) {
        ssize_t random = read(urandomFile, buffer, sizeof(char) * recordSize);
        if(random <0){
            perror("Error during reading /dev/urandom file");
            exit(1);
        }
        ssize_t wNewFile = write(newFile,buffer, sizeof(char) * recordSize);
        if (wNewFile<0){
            perror("Error during writing new file");
            exit(1);
        }

    }
    free(buffer);
    close(urandomFile);
    close(newFile);
}
void shuffle_sys(char* fileName, int noRecords, int recordSize) {
    int file = open(fileName, O_RDWR);
    if (file < 0){
        perror(fileName);
        exit(1);
    }
    int x = 0;
    char *buffer = calloc((size_t) recordSize, sizeof(char));
    char *tmp = calloc((size_t) recordSize, sizeof(char));
    for (int j = noRecords-1; j >0; --j) {
        x = rand()%j;
        lseek(file, j * (recordSize), SEEK_SET);
        read(file,buffer,sizeof(char)*recordSize);
        lseek(file, x * (recordSize), SEEK_SET);
        read(file, tmp, sizeof(char)*recordSize);
        lseek(file, x * (recordSize), SEEK_SET);
        write(file, buffer,sizeof(char)*recordSize);
        lseek(file, j * (recordSize), SEEK_SET);
        write(file, tmp,sizeof(char)*recordSize);
        if (file < 0){
            perror(fileName);
            exit(1);
        }

    }
    if (close(file) < 0){
        perror(fileName);
        exit(1);
    }
}
void shuffle_lib(char* fileName, int noRecords, int recordSize) {
    FILE * file = fopen(fileName, "r+");
    if (file==NULL){
        perror(fileName);
        exit(1);
    }
    char * buffer = calloc((size_t) recordSize, sizeof(char));
    char * tmp = calloc((size_t) recordSize, sizeof(char));
    int x =0;
    for (int j = noRecords-1; j >0; --j) {
        x = rand()%j;
        fseek(file, j * (recordSize), SEEK_SET);
        fread(buffer, sizeof(char), (size_t) recordSize, file);
        fseek(file, x * (recordSize), SEEK_SET);
        fread(tmp, sizeof(char), (size_t) recordSize, file);
        fseek(file, x * (recordSize), SEEK_SET);
        fwrite(buffer, sizeof(char), (size_t) recordSize, file);
        fflush(file);
        fseek(file, j * (recordSize), SEEK_SET);
        fwrite(tmp, sizeof(char), (size_t) recordSize, file);
        fflush(file);

    }
    fclose(file);
}

void sort_sys(char * fileName, int noRecords, int recordSize){
    int file = open(fileName,O_RDONLY);
    if (file < 0){
        perror(fileName);
        exit(1);
    }
    char * newFileName = malloc(25);
    char * begining = malloc(20);
    for (int k = 0; fileName[k] != '\0' && fileName[k] != '.'; ++k) {
        begining[k]=fileName[k];
    }
    sprintf(newFileName, "%s_sorted_sys.txt", begining);
    int newFile = open(newFileName, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH |S_IWUSR | S_IWGRP | S_IWOTH);
    char * buffer = calloc((size_t) recordSize, sizeof(char));
    char * buffer2 = calloc((size_t) recordSize, sizeof(char));
    for (int i = 0; i < noRecords; ++i) {
        read(file,buffer,sizeof(char)*recordSize);
        if (file < 0){
            perror(fileName);
            exit(1);
        }
        write(newFile,buffer,sizeof(char)*recordSize);
        if (newFile < 0){
            perror(newFileName);
            exit(1);
        }
    }
    close(file);
    lseek(newFile,0,SEEK_SET);
    beginMeasurement();
    for (int i = 0; i < noRecords; ++i) {
        for (int j = 0; j < noRecords - i -1; ++j) {
            lseek(newFile,j*recordSize,SEEK_SET);
            read(newFile,buffer,sizeof(char)*recordSize);
            read(newFile, buffer2, sizeof(char)*recordSize);
            if (buffer[0]>buffer2[0]){
                lseek(newFile,(j+1)*recordSize,SEEK_SET);
                write(newFile,buffer,sizeof(char)*recordSize);
                lseek(newFile,j*recordSize,SEEK_SET);
                write(newFile, buffer2,sizeof(char)*recordSize);
            }
            if (newFile < 0){
                perror(newFileName);
                exit(1);
            }
        }

        lseek(newFile,0,SEEK_SET);
    }
    finishMeasurement();
    printTime();
    if (close(newFile) < 0){
        perror(newFileName);
        exit(1);
    }
}
void sort_lib(char * fileName, int noRecords, int recordSize){
    FILE * file = fopen(fileName,"r");
    if (file == NULL){
        perror(fileName);
        exit(1);
    }
    char * newFileName = malloc(25);
    char * begining = malloc(20);
    for (int k = 0; fileName[k] != '\0' && fileName[k] != '.'; ++k) {
        begining[k]=fileName[k];
    }
    sprintf(newFileName, "%s_sorted_lib.txt", begining);
    FILE * newFile = fopen(newFileName,"w+");
    char * buffer = calloc((size_t) recordSize, sizeof(char));
    char * buffer2 = calloc((size_t) recordSize, sizeof(char));
    for (int i = 0; i < noRecords; ++i) {
        fread(buffer, sizeof(char), (size_t) recordSize, file);
        fwrite(buffer, sizeof(char), (size_t) recordSize, newFile);
    }
    fclose(file);
    beginMeasurement();

    for (int i = 0; i < noRecords; ++i) {
        for (int j = 0; j < noRecords - i - 1; ++j) {
            fseek(newFile,j*recordSize,SEEK_SET);
            fread(buffer, sizeof(char), (size_t) recordSize, newFile);
            fread(buffer2, sizeof(char), (size_t) recordSize, newFile);
            if (buffer[0]>buffer2[0]){
                fseek(newFile,(j+1)*recordSize,SEEK_SET);
                fwrite(buffer, sizeof(char), (size_t) recordSize, newFile);
                fseek(newFile, j*recordSize,SEEK_SET);
                fwrite(buffer2, sizeof(char), (size_t) recordSize, newFile);
            }
        }
    }
    finishMeasurement();
    printTime();

}




int main(int argc, char ** argv) {
    char * fileName;
    int records;
    int sizeOfRecord = 0;
    if (argc <5) {
        errno = 22;
        perror("To few arguments");
        exit(1);
    }
    if (argc>6) {
        errno = 7;
        perror("There should be no more than 6 arguments");
        exit(1);
    }
    if( strcmp(argv[1], "generate")==0){
        fileName = argv[2];
        records = atoi(argv[3]);
        sizeOfRecord=atoi(argv[4]);
        beginMeasurement();
        generateFile(fileName,records,sizeOfRecord);
        finishMeasurement();
        printTime();
    } else if (strcmp(argv[1],"sys")==0){
        if(strcmp(argv[2], "shuffle")==0){
            fileName = argv[3];
            records = atoi(argv[4]);
            sizeOfRecord=atoi(argv[5]);
            beginMeasurement();
            shuffle_sys(fileName,records,sizeOfRecord);
            finishMeasurement();
            printTime();
        } else if (strcmp(argv[2], "sort")==0){
            fileName = argv[3];
            records = atoi(argv[4]);
            sizeOfRecord=atoi(argv[5]);
            sort_sys(fileName,records,sizeOfRecord);

        } else {
            errno  = 22;
            perror("Second argument should be shuffle or sort");
        };
    } else if (strcmp(argv[1],"lib")==0){
        if(strcmp(argv[2], "shuffle")==0){
            fileName = argv[3];
            records = atoi(argv[4]);
            sizeOfRecord=atoi(argv[5]);
            beginMeasurement();
            shuffle_lib(fileName,records,sizeOfRecord);
            finishMeasurement();
            printTime();
        } else if (strcmp(argv[2], "sort")==0){
            fileName = argv[3];
            records = atoi(argv[4]);
            sizeOfRecord=atoi(argv[5]);
            sort_lib(fileName,records,sizeOfRecord);

        }else {
            errno  = 22;
            perror("Second argument should be shuffle or sort");
        };
    } else {
        errno  = 22;
        perror("First argument should be generate, lib or sys");
    };

    return 0;
}
