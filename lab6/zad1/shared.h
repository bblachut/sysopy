#ifndef SYSOPY_SHARED_H
#define SYSOPY_SHARED_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define PROJECT_ID 'P'
#define MAX_CLIENTS 10
#define MAX_MESSAGE_LENGTH 256
#define HOME getenv("HOME")

#define STOP 1
#define LIST 2
#define TOALL 3
#define TOONE 4
#define INIT 5

typedef struct {
    long type;    // message's type as specified by the sending process
    char text[MAX_MESSAGE_LENGTH];
    int q_id;
    int sender_id;
    int to_connect_id;
} message;

typedef struct{
    int id;
    int queue_id;
    int IS_CONNECTED;
} client;

void delete_queue(int q) {
    msgctl(q, IPC_RMID, NULL);
}

void send_message(message *msg, int send_to){
    if(msgsnd(send_to, msg, sizeof(message) - sizeof (long), 0) == -1){
        printf("msgsnd\n");
        exit(1);
    }
}

void receive_message(int q_id, message* msg, long type){
    if(msgrcv(q_id, msg, sizeof(message) - sizeof(long), type,0) == -1){
        printf("msgrcv\n");
        exit(1);
    }
}

#endif //SYSOPY_SHARED_H
