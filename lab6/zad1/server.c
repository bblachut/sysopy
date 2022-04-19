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
            clients[i].queue_id = msg->q_id;
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
    // send stop to all connected clients
    // then should receive STOP back
    printf("STOPPING SERVER...\n");
    message msg;
    int client_q;
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].IS_CONNECTED) {
            client_q = clients[i].queue_id;
            msg.type = STOP;
            send_message(&msg, client_q);

            receive_message(qid, &msg, STOP);
            printf("STOP received from client\n");
        }
    }

    delete_queue(qid);
    printf("\nSERVER STOPPED!\n");
}

void handle_STOP(message* msg){
    clients[msg->sender_id].IS_CONNECTED = 0;
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

int main(int argc, char ** argv) {

    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].IS_CONNECTED = 0;
    }

    key_t msg_queue_key;

    //server queue
    if((msg_queue_key = ftok(HOME, PROJECT_ID)) == -1) {
        perror("ftok");
        exit(1);
    }
    //printf("key_t: %d\n", msg_queue_key);
    if((qid = msgget(msg_queue_key, IPC_CREAT | 0666)) == -1) {
        perror("msget");
        exit(1);
    }

    //printf("qid: %d\n", qid);
    signal(SIGINT, stop_server);


    message msg;

    while(1) {
        //recieving
        if(msgrcv(qid, &msg, sizeof (message), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("Message recieve\n");
        fflush(stdout);
        choose_mode(&msg);
    }

}