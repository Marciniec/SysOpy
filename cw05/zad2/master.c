#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <math.h>

int R;
int ** D;
void plot_graph(){
    FILE* file = fopen("data","w");
    for(int i=0;i<R;++i) {
        for(int j=0;j<R;++j) {
            fprintf(file,"%d %d %d\n",i,j,D[i][j]);
        }
    }
    fclose(file);
    FILE* graph = popen("gnuplot","w");
    fprintf(graph,"set view map\n");
    fprintf(graph, "set xrange [0:%d]\n",R-1);
    fprintf(graph, "set yrange [0:%d]\n",R-1);
    fprintf(graph, "plot 'data' with image\n");

    fflush(graph);
    getc(stdin);
    pclose(graph);
}

int main(int argc, char ** argv) {
    if(argc != 3){
        printf("Wrong number of arguments");
        exit(1);
    }
    char * pipe_name = argv[1];
    mkfifo(pipe_name,0666);
    int pipe = open(pipe_name,O_RDONLY);

    R = atoi(argv[2]);
    D = malloc(sizeof(int *)*R);
    for (int i = 0; i < R; ++i) {
        D[i] = calloc((size_t) R, sizeof(int));
    }
    double im, re;
    int iter;
    char buff[51];
    char * token;
    while (read(pipe,buff,51)>0){
        token = strtok(buff," ");
        re = atof(token);
        token = strtok(NULL," ");
        im = atof(token);
        token = strtok(NULL," ");
        iter = atoi(token);
        int x = (int)(R*(fabs(-2.0-re)/3.0));
        int y = (int)(R*(fabs(1.0 -im)/2.0));
        D[x][y]=iter;
    }
    close(pipe);
    unlink(pipe_name);
    plot_graph();
    return 0;
}
