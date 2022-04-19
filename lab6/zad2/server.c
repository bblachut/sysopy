#include "shared.h"

client clients[MAX_CLIENTS];
int qid;

void handle_INIT(message *msg) {
    int slot = -1;
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].IS_CONNECTED == 0) {
            slot = i;
            clients[i].IS_CONNECTED = 1;
            clients[i].id = i;
            strcpy(clients[i].queue_name, msg->text);
            clients[i].queue_id = mq_open(clients[i].queue_name, O_WRONLY);
            break;
        }
    }
    if(slot == -1) {
        printf("NO MORE SLOTS FOR CLIENTS\n");
        return;
    }

    printf("SERVER: Init client with id: %d\n", slot);
    message response = {.type = INIT, .sender_id = slot};

    send_message(&response, clients[slot].queue_id);
}

void stop_server(){
    printf("STOPPING SERVER...\n");
    message msg;
    msg.type = STOP;
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].IS_CONNECTED) {
            send_message(&msg, clients[i].queue_id);

            mq_close(clients[i].queue_id);
        }
    }

    mq_close(qid);
    mq_unlink(SERVER_Q);
    printf("\nSERVER STOPPED!\n");
    exit(0);
}

void handle_STOP(message* msg){
    clients[msg->sender_id].IS_CONNECTED = 0;
    mq_close(clients[msg->sender_id].queue_id);
    clients[msg->sender_id].queue_id = -1;
}

void handle_TOONE(message* msg){
    if (clients[msg->to_connect_id].IS_CONNECTED){
        send_message(msg, clients[msg->to_connect_id].queue_id);
    }
    printf("Message sent\n");
}

void handle_TOALL(message* msg){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].IS_CONNECTED && i != msg->sender_id){
            send_message(msg, clients[i].queue_id);
        }
    }
    printf("Messages sent\n");
}

void handle_LIST(){
    printf("List of clients:\n");
    for(int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].IS_CONNECTED){
            printf("Client with ID: %d\n", i);
        }
    }
}

void choose_mode(message *msg) {
    switch (msg->type) {
        case INIT:
            handle_INIT(msg);
            break;
        case LIST:
            handle_LIST();
            break;
        case TOALL:
            handle_TOALL(msg);
            break;
        case TOONE:
            handle_TOONE(msg);
            break;
        case STOP:
            handle_STOP(msg);
            break;
        default:
            printf("WRONG MESSAGE TYPE\n");
    }
}

int main() {

    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].IS_CONNECTED = 0;
    }

    struct mq_attr attr = {.mq_maxmsg = MAX_MESSAGES, .mq_msgsize = sizeof(message), .mq_flags = 0, .mq_curmsgs = 0};
    if((qid = mq_open(SERVER_Q,  O_RDWR | O_CREAT, 0666, &attr)) == -1){
        printf("mq_open");
        exit(1);
    }


    printf("qid: %d\n", qid);
    signal(SIGINT, stop_server);


    message msg;
    unsigned int type;

    while(1) {
        receive_message(qid, &msg, &type);
        printf("Message recieved\n");
        fflush(stdout);
        choose_mode(&msg);
    }

}