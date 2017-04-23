#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <math.h>
#include <complex.h>
#include <time.h>
double Re(){
    return (((double)rand()/(double)RAND_MAX)*3.0-2.0);
}
double Im(){
    return (((double)rand()/(double)RAND_MAX)*2.0-1.0);
}


int main(int argc, char ** argv){
    srand((unsigned int)time(NULL));
    if(argc!=4){
        printf("Wrong number of arguments");
        exit(1);
    }
    char * pipe_name = argv[1];
    int N = atoi(argv[2]);
    int K = atoi(argv[3]);
    int file = open(pipe_name, S_IFIFO | O_WRONLY);
    int iter;
    double complex z0;
    double complex c;
    char buff[51];
    for (int i = 0; i < N; ++i) {
        c = Re() + Im()*I;
        z0 = 0;
        iter = 0;
        while (cabs(z0)<=2 && iter <K){
            z0 = z0 *z0 +c;
            iter++ ;
        }
        sprintf(buff,"%lf %lf %d", creal(c), cimag(c), iter);
        write(file,buff,51);
    }
    close(file);
    return 0;
}