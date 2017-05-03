#ifndef ZADANIE1_ZADANIE_H
#define ZADANIE1_ZADANIE_H
#define MAX_CLIENTS 20
#define MAX_LEN_MSG 100

int small_numbers[24] ={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','r','s','t','q','u','v','w','z'};
/* message buffer for msgsnd and msgrcv calls */
typedef enum message_type{
    LOGIN , ECHO , END, REFUSE
}message_type;


struct msgbuf {
    long mtype;         /* typ komunikatu   */
    char mtext[MAX_LEN_MSG];      /* tresc komunikatu */
    pid_t origin_pid;
    key_t client_queue_key;
    int client_id;
    message_type msg_type;
};


#endif //ZADANIE1_ZADANIE_H
