#include <stdio.h>
#include  <mqueue.h>
#include <stdlib.h>
#include <unistd.h>
#include "zadanie.h"

mqd_t server_queue_id;
mqd_t client_queue_id;
char * clients[MAX_CLIENTS];
int client_index;
int end;
void receive_message(char * message){
    char * send_message = malloc(MAX_MSG_SIZE);
    int client_id;
    switch (message[0]){
        case 'a':
            message++;
            printf("Server received login message %s from client %d\n", message,client_index);
            if(client_index < MAX_CLIENTS) {

                sprintf(clients[client_index],"%s",message);
                sprintf(send_message,"a%d",client_index);
                if((client_queue_id = mq_open(message,O_WRONLY))==-1){
                    perror("Error during opening client queue (login)");
                    exit(1);
                }
                printf("Server's sending accept message to client: %d\n",client_index);
                if(mq_send(client_queue_id,send_message,MAX_MSG_SIZE,1)==-1){
                    perror("Error during sending accept message to client ");
                    exit(1);
                }
            } else{
                sprintf(send_message,"%c",'d');
                if((client_queue_id = mq_open(message,O_WRONLY))==-1){
                    perror("Error during opening client queue(login)");
                    exit(1);
                }
                printf("Server's sending refuse message to client %d\n", client_index);
                if(mq_send(client_queue_id,send_message,MAX_MSG_SIZE,1)==-1){
                    perror("Error during sending refuse message to client");
                    exit(1);
                }

            }
            client_index++;
            break;
        case 'b':
            client_id=message[1] - '0';
            message+=2;
            sprintf(send_message,"b%d%s",client_id,message);
            printf("Received ECHO Message: \"%s\" from client: %d \n", message, client_id);
            if((client_queue_id = mq_open(clients[client_id],O_WRONLY))==-1){
                perror("Error during opening client queue(ECHO)");
                exit(1);
            }
            printf("Sending ECHO message back to Client %d\n",client_id);
            if(mq_send(client_queue_id,send_message,MAX_MSG_SIZE,1)==-1){
                perror("Error during sending to client(ECHO)");
                exit(1);
            }

            break;
        case 'c':
            client_id= message[1] - '0';
            message+=2;
            printf("Received END message \"%s\" from client: %d\n", message,client_id);
            sprintf(send_message,"c%d%s",client_id,"Good Bye!");

            if((client_queue_id = mq_open(clients[client_id],O_WRONLY))==-1){
                perror("Error during opening client queue(END)");
                exit(1);
            }
            printf("Sending farewell message to client %d\n",client_id);
            if(mq_send(client_queue_id,send_message,MAX_MSG_SIZE,1)==-1){
                perror("Error during sending to client(END)");
                exit(1);
            }
            clients[client_id][0]='\0';
            end = 1;
            break;
        default:break;
    }

}


int main() {

    struct mq_attr server_stats;
    server_stats.mq_maxmsg = 10;
    server_stats.mq_msgsize = MAX_MSG_SIZE;
    end = 0;
    for (int i = 0; i < 10; ++i) {
        clients[i] = malloc(MAX_MSG_SIZE);
    }
    server_queue_id = mq_open(SERVER_NAME, O_CREAT | O_RDONLY,  0644, &server_stats);
    printf("SERVER ID : %d \n", server_queue_id);
    char *message = malloc(MAX_MSG_SIZE);
    int prior =1;
    while (!end || server_stats.mq_curmsgs) {
        mq_receive(server_queue_id, message, MAX_MSG_SIZE, (unsigned int *) &prior);
        receive_message(message);
        mq_getattr(server_queue_id,&server_stats);
        sleep(2);
    }
    while (clients[client_index-1][0]!='\0'){
        mq_receive(server_queue_id, message, MAX_MSG_SIZE, (unsigned int *) &prior);
        receive_message(message);
        mq_getattr(server_queue_id,&server_stats);
        sleep(2);
    }
    mq_close(server_queue_id);
    mq_unlink(SERVER_NAME);
    return 0;
}