#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <memory.h>
#include <unistd.h>
#include <string.h>
#include "zadanie.h"
int end;
int client_queue;
int client_index;
int server_id;
pid_t clients_pid[MAX_CLIENTS];

void receive_message(struct msgbuf message){
    struct msgbuf send_message;
    send_message.mtype = 1;
    switch (message.msg_type){
        case LOGIN:
            end = 0;
            printf("Registering client %d \n", client_index);
            if(client_index >= MAX_CLIENTS){
                printf("Server %d is receiving key from client with PID : %d and id: %d \n", server_id,
                       message.origin_pid, client_index);
                if ((client_queue = msgget(message.client_queue_key, IPC_PRIVATE | 0666)) == -1) {
                    perror("Error during getting client queue ");
                    exit(1);
                }
                send_message.client_id = client_index;
                sprintf(send_message.mtext, "There's to much clients \n");
                send_message.msg_type = REFUSE;
                printf("Server is sending refuse message to client %d \n", client_queue);
                msgsnd(client_queue, &send_message, sizeof(struct msgbuf), 0);
            }else {

                clients_pid[client_index] = message.origin_pid;
                printf("Server %d is receiving key from client with PID : %d and id: %d \n", server_id,
                       message.origin_pid, client_index);
                if ((client_queue = msgget(message.client_queue_key, IPC_PRIVATE | 0666)) == -1) {
                    perror("Error during getting client queue ");
                    exit(1);
                }
                send_message.client_id = client_index;
                sprintf(send_message.mtext, "Your ID is : %d", client_index);
                send_message.msg_type = LOGIN;
                printf("Server is sending accept message to client %d \n", client_index);
                msgsnd(client_queue, &send_message, sizeof(struct msgbuf), 0);
                client_index++;
            }
            break;
        case ECHO:
            printf("Server received message: \" %s \" from client: %d \n",message.mtext, message.client_id);
            send_message.mtype =1;
            strcpy(send_message.mtext, message.mtext);
            if ((client_queue = msgget(message.client_queue_key, IPC_PRIVATE | 0666)) == -1) {
                perror("Error during getting client queue ");
                exit(1);
            }
            printf("Server is sending back message: \" %s \" to client: %d \n",send_message.mtext, message.client_id);
            msgsnd(client_queue,&send_message,sizeof(struct msgbuf),0);
            break;
        case END:
            printf("Server received end message form client : %d \n", message.client_id);
            if ((client_queue = msgget(message.client_queue_key, IPC_PRIVATE | 0666)) == -1) {
                perror("Error during getting client queue ");
                exit(1);
            }
            strcpy(send_message.mtext, "Good bye");
            send_message.mtype = 1;
            send_message.msg_type = END;
            msgsnd(client_queue,&send_message,sizeof(struct msgbuf),0);
            clients_pid[message.client_id]=-1;
            end =1;
        default:break;
    }


}

int main() {
    char * home = getenv("HOME");
    key_t queue_key = ftok(home,'a');
    if((server_id =  msgget(queue_key,  IPC_CREAT )) == -1 ){
        perror("Error msgget");
        exit(1);
    }
    end = 0;
    printf("%d \n" ,server_id);
    struct msgbuf  ptr;
    struct msqid_ds  stats;
    while (!end || stats.msg_qnum) {
        if(msgrcv(server_id,&ptr,sizeof(struct msgbuf),0,0)==-1){
            perror("Error msgrcv");
            exit(1);
        }
        sleep(2);
        msgctl(server_id,IPC_STAT,&stats);
        receive_message(ptr);
    }
    while (clients_pid[client_index-1]!=-1){
        if(msgrcv(server_id,&ptr,sizeof(struct msgbuf),0,0)==-1){
            perror("Error msgrcv");
            exit(1);
        }
        msgctl(server_id,IPC_STAT,&stats);
        receive_message(ptr);
    }
    msgctl(server_id,IPC_RMID,&stats);
    return 0;
}
