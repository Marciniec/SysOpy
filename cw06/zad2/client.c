#include <stdio.h>
#include <mqueue.h>
#include "zadanie.h"
#include <unistd.h>
#include <stdlib.h>
mqd_t server_queue_id;
mqd_t client_queue_id;
int my_id;
char client_name[20];
int refuse;
void receive_messages(){
    char * message = malloc(MAX_MSG_SIZE);
    unsigned int prio = 1;
    if( mq_receive(client_queue_id,message,MAX_MSG_SIZE, &prio)==-1){
        perror("Error during receiving messages from server");
        exit(1);
    }
    switch (message[0]){
        case 'a':
            message++;
            my_id = atoi(message);
            printf("Received accept message from server my id is: %d\n", my_id);
            break;
        case 'b':
            message+=2;

            printf("Received ECHO message from server : \"%s\"\n",message);
            break;
        case 'c':
            message+=2;
            printf("Received END message from server : \"%s\"\n",message);
            break;
        case 'd':
            printf("Received REFUSE message from server \n");
            refuse = 1;
            break;
        default:
            break;
    }
}



void send_messages(){
    char * message= malloc(MAX_MSG_SIZE);
    sprintf(message,"a%s",client_name);
    printf("Sending client name %s \n",client_name);
    if ( mq_send(server_queue_id,message,MAX_MSG_SIZE,1 )==-1){
        perror("Error during sending login message");
        exit(1);
    }
    receive_messages();
    free(message);
    if(!refuse) {
        message = malloc(MAX_MSG_SIZE);
        sprintf(message, "b%d%s", my_id, "Super important message");
        if (mq_send(server_queue_id, message, MAX_MSG_SIZE, 1) == -1) {
            perror("Error during sending login message");
            exit(1);
        }
        receive_messages();
        free(message);
        message = malloc(MAX_MSG_SIZE);
        sprintf(message, "c%d%s", my_id, "Bye!");
        if (mq_send(server_queue_id, message, MAX_MSG_SIZE, 1) == -1) {
            perror("Error during sending login message");
            exit(1);
        }
        receive_messages();
    }
}


int main(){
    refuse = 0;
    struct mq_attr client_stats;
    client_stats.mq_maxmsg = 10;
    client_stats.mq_msgsize = MAX_MSG_SIZE;
    sprintf(client_name,"/client-%d",getpid());
    server_queue_id = mq_open(SERVER_NAME,O_WRONLY);
    client_queue_id = mq_open(client_name, O_CREAT | O_RDONLY, 0644,&client_stats);
    send_messages();
    mq_close(client_queue_id);
    mq_close(server_queue_id);
    mq_unlink(client_name);
    return 0;
}