#include "shared.h"

int server_q;
int client_q;
int id;

char* get_name(){
    char* name = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
    sprintf(name, "/client_q_%d", getpid());
    return name;
}

void send_INIT(){
    printf("Client send INIT\n");
    message msg = {.type = INIT};

    strcpy(msg.text, get_name());
    send_message(&msg, server_q);
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

void handle_INIT(message* msg){
    id = msg->sender_id;
    printf("--- CLIENT ID ---\n%d\n\n", id);
}

void handle_text(message* msg){
    printf("New message from %d\n", msg->sender_id);
    printf("%s\n", msg->text);
}

void  handle_STOP(){
    printf("Received STOP from server\n");
    mq_close(client_q);
    mq_close(server_q);
    mq_unlink(get_name());
    exit(0);
}

int is_empty(int q){
    struct mq_attr attr;
    mq_getattr(q, &attr);
    if (attr.mq_curmsgs == 0) return 1;
    return 0;
}

void catcher(){

    while (is_empty(client_q) == 0){
        message msg;
        unsigned int type;
        receive_message(client_q, &msg, &type);

        switch (msg.type)
        {
            case STOP:
                handle_STOP();
                break;
            case INIT:
                handle_INIT(&msg);
                break;
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

    server_q = mq_open(SERVER_Q, O_WRONLY);


    struct mq_attr attr = {.mq_maxmsg = MAX_MESSAGES, .mq_msgsize = sizeof(message), .mq_flags = 0, .mq_curmsgs = 0};
    if((client_q = mq_open(get_name(),  O_RDWR | O_CREAT | O_NONBLOCK, 0666, &attr)) == -1){
        printf("mq_open");
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