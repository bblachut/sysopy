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
#include <mqueue.h>
#include <fcntl.h>
#include <errno.h>


#define SERVER_Q "/server_q1"
#define MAX_CLIENTS 10
#define MAX_MESSAGES 10
#define MAX_MESSAGE_LENGTH 256

#define STOP 1
#define LIST 2
#define TOALL 3
#define TOONE 4
#define INIT 5

typedef struct {
    long type;    // message's type as specified by the sending process
    char text[MAX_MESSAGE_LENGTH];
    int sender_id;
    int to_connect_id;
} message;

typedef struct{
    int id;
    char queue_name[MAX_MESSAGE_LENGTH];
    int queue_id;
    int IS_CONNECTED;
} client;


void send_message(message* msg, int q_id){
    if (mq_send(q_id, (char *) msg, sizeof(message), msg->type) == -1){
        printf("Error while sending message! %s\n", strerror(errno));
        exit(1);
    }
}

void receive_message(int q_id, message* msg, unsigned int* type){
    if(mq_receive(q_id, (char *) msg, sizeof(message), type) == -1){
        printf("Error while receiving message! %s\n", strerror(errno));
        exit(1);
    }
}

#endif //SYSOPY_SHARED_H
