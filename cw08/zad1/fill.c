#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <stdio.h>

struct record{
    int id;
    char text[1020];
};
int main(){
    int filedes;
    filedes = open("text.txt",O_CREAT|O_RDWR);
    if(filedes ==-1){
        perror("Error during opening text.txt");
        exit(1);
    }
    struct record *a = malloc(sizeof(struct record));
    int urandomFile = open("/dev/urandom",O_RDONLY);
    if(urandomFile==-1){
        perror("Error during opening urandom");
        exit(1);
    }
    char * buffer = malloc(sizeof(char)*1020);
    ssize_t random = read(urandomFile, buffer, sizeof(char) * 1020);
    if(random<0){
        perror("Error during reading from urandom");
        exit(1);
    }
    for (int i = 0; i < 200; ++i) {
        a->id = i;
        urandomFile = open("/dev/urandom",O_RDONLY);
        if(urandomFile==-1){
            perror("Error during opening urandom");
            exit(1);
        }
        buffer = malloc(sizeof(char)*1020);
        if(i!=10  && i!=25) {
            random = read(urandomFile, buffer, sizeof(char) * 1020);
            if(random<0){
                perror("Error during reading from urandom");
                exit(1);
            }
        } else {
            buffer = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc enim diam, wicked ut scelerisque non, placerat ut ipsum. Integer fringilla risus risus, quis dictum augue sodales id. Nullam euismod, ligula et lobortis tincidunt, SysOpy elit interdum ipsum, a pretium enim ligula eu magna. Donec egestas dui non hendrerit venenatis. In eget ex sed purus dignissim imperdiet sit amet et odio. Praesent sagittis sapien felis, vitae accumsan tellus bibendum et. Aenean tempus accumsan porta.\n"
                    "\n"
                    "Maecenas ac nunc nibh. Cras urna nunc, suscipit et slowo eget, tempor volutpat ex. Curabitur urna mauris, lobortis posuere lacinia non, sodales eget erat. Vivamus sed tincidunt libero. Curabitur sed augue eu justo aliquet interdum. Duis vestibulum rhoncus nisl, vitae pellentesque ligula maximus nec. Sed et accumsan erat. Fusce eleifend erat ac metus ullamcorper suscipit. Suspendisse eleifend eleifend libero, eu mollis arcu faucibus quis. Nulla quis cursus risus, ac iaculis justo. Vestibulum bibendum a dui eu volutpat orci aliquam.";
        }
        strcpy(a->text,buffer);
        if(write(filedes,a,1024)==-1){
            perror("Error during writing into text.txt");
            exit(1);
        }
    }
    close(filedes);
    return 0;
}