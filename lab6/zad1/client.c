#include "shared.h"

int server_q;
int client_q;
int id;


void send_INIT(){
    printf("Client send INIT\n");
    message msg = {.q_id = client_q, .type = INIT};
    send_message(&msg, server_q);

    message received;
    receive_message(client_q, &received, INIT);
    printf("--- CLIENT ID ---\n%d\n\n", received.sender_id);
    id = received.sender_id;
}

void send_LIST(){
    printf("Client send LIST\n");
    message msg = {.type = LIST};
    send_message(&msg, server_q);
}

void send_STOP(){
    printf("Client send STOP\n");
    message msg = {.sender_id = id, .type = STOP};
    send_message(&msg, server_q);
    msgctl(client_q, IPC_RMID, NULL);
    exit(0);
}

void send_TOALL(char* text){
    printf("Client send 2ALL\n");
    message msg = {.sender_id = id, .type = TOALL};
    strcpy(msg.text, text);

    send_message(&msg, server_q);
}

void send_TOONE(char* text, int send_to){
    printf("Client send 2ONE\n");
    message msg = {.sender_id = id, .type = TOONE, .to_connect_id = send_to};
    strcpy(msg.text, text);

    send_message(&msg, server_q);
}

void handle_text(message* msg){
    printf("New message from %d\n", msg->sender_id);
    printf("%s\n", msg->text);
}

void  handle_STOP(){
    printf("Received STOP from server");
    send_STOP();
}

int is_empty(int q){
    struct msqid_ds buf;
    msgctl(q, IPC_STAT, &buf);

    if (buf.msg_qnum != 0) return 0;
    else return 1;
}

void catcher(){

    while (is_empty(client_q) == 0){
        message msg;
        if(msgrcv(client_q, &msg, sizeof(message) - sizeof(long), -6, IPC_NOWAIT) == -1){
            printf("Error while receiving message (NOWAIT)!\n");
            exit(1);
        }

        switch (msg.type)
        {
            case STOP:
                handle_STOP();
            case TOALL:
                handle_text(&msg);
                break;
            case TOONE:
                handle_text(&msg);
                break;
            default:
                break;
        }
    }
}


int check_input(){
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return (FD_ISSET(0, &fds));
}

int main(){
    signal(SIGINT, send_STOP);

    key_t client_key, msg_queue_key;

    if((client_key = ftok(HOME, getpid())) == -1) {
        perror("ftok");
        exit(1);
    }
    if((client_q = msgget(client_key,  IPC_CREAT | IPC_EXCL | 0666)) == -1) {
        perror("client_q");
        exit(1);
    }

    if((msg_queue_key = ftok(HOME, PROJECT_ID)) == -1) {
        perror("ftok");
        exit(1);
    }
    //printf("key_t: %d\n", msg_queue_key);
    if((server_q = msgget(msg_queue_key, IPC_CREAT | 0666)) == -1) {
        perror("msget");
        exit(1);
    }

    printf("client_qid: %d\n", client_q);

    send_INIT();

    char buffer[MAX_MESSAGE_LENGTH];

    while(1){
        if (check_input()) {
            fgets(buffer, MAX_MESSAGE_LENGTH, stdin);

            if (strcmp("LIST\n", buffer) == 0) {
                send_LIST();
            } else if (strcmp("STOP\n", buffer) == 0) {
                send_STOP();
            } else if (strcmp("2ONE\n", buffer) == 0) {
                printf("Enter client ID: ");
                fgets(buffer, MAX_MESSAGE_LENGTH, stdin);
                int id_to_sent = atoi(buffer);
                printf("Enter message text: ");
                fgets(buffer, MAX_MESSAGE_LENGTH, stdin);
                send_TOONE(buffer, id_to_sent);
            } else if (strcmp("2ALL\n", buffer) == 0) {
                printf("Enter message text: ");
                fgets(buffer, MAX_MESSAGE_LENGTH, stdin);
                send_TOALL(buffer);
            }
            else {
                printf("Unknown command: %s\n", buffer);
            }
        }

        sleep(1);
        catcher();
    }
}