#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "zadanie.h"
#include <sys/types.h>
#include <string.h>
int server_id;
int client_id;
key_t queue_key;
int my_id;
int refuse;

void send_queue_id(int server_id, key_t queue_key){
    struct msgbuf message;
    message.mtype=0;
    message.origin_pid = getpid();
    message.client_queue_key = queue_key;
    printf("Sending client queue key: %d  to server : %d \n",queue_key,server_id);
    if( msgsnd(server_id,&message, sizeof(struct msgbuf),0)==-1){
        perror("Error during sending client queue id to server");
        exit(1);
    }
}

void receive_messages(){
      struct msgbuf message;
      printf("Receiving message \n");
      msgrcv(client_id,&message,sizeof(struct msgbuf),0,0);
      switch (message.msg_type){
          case LOGIN:
              printf("Received permission from server\n");
              my_id = message.client_id;
              printf("Server message \" %s \" \n", message.mtext);
              break;
          case ECHO:
              printf("Echo from server \" %s \"", message.mtext);
              break;
          case REFUSE:
              printf("No permission from server \n");
              printf("Server message \" %s \" \n", message.mtext);
              refuse = 1;
              break;
          case END:
              printf("Server message \" %s \" \n", message.mtext);
              break;
          default:
              break;
      }
}
void send_messages(){
    struct msgbuf message;
    message.mtype = 1;
    message.msg_type =LOGIN;
    message.origin_pid = getpid();
    message.client_queue_key = queue_key;
    message.client_id = my_id;
    printf("Sending client IPC key to server \n");
    msgsnd(server_id,&message,sizeof(message),0);
    receive_messages();
    if(!refuse) {
        printf("Sending ECHO request to server \n");
        strcpy(message.mtext, "Super important message");
        message.msg_type = ECHO;
        message.client_id = my_id;
        msgsnd(server_id, &message, sizeof(message), 0);
        receive_messages();
        message.msg_type = END;
        printf("Sending END request to server \n");
        strcpy(message.mtext, "Bye!");
        msgsnd(server_id, &message, sizeof(message), 0);
        receive_messages();
    }
}




int main() {
    srand(time(NULL));
    char * home = getenv("HOME");
    queue_key = ftok(home,small_numbers[rand()%23]);
    key_t server_queue_key = ftok(home,'a');
    struct msqid_ds stats;
    if( (client_id = msgget(queue_key,IPC_CREAT| 0666 ))==-1){
        perror("Error during opening client queue");
        exit(1);
    }
    if ((server_id = msgget(server_queue_key, 0))==-1){
        perror("Opening server ");
        exit(1);
    }
    send_messages();
    msgctl(client_id,IPC_RMID,&stats);


    return 0;
}
