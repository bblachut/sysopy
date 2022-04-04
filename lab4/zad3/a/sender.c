#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int caught = 0;
int to_send;
char* mode;

void handler1(int signum){
    caught++;
}

void handler2(int sig, siginfo_t *info, void *ucontext){
    printf("\n--------------------------- ALL SIGNALS SENT BACK --------------------------------\n\n");
    printf("Sender handler2 here. SIGUSR2 caught.\n");
    printf("\nSender caught [%d] SIGUSR signals, should have caught [%d].\n", caught, to_send);
    exit(0);
}

int main(int argc, char* argv[]){

    if (argc != 4){
        printf("Wrong number of arguments!\n");
        return -1;
    }


    int catcher_PID = atoi(argv[1]);
    to_send = atoi(argv[2]);
    mode = argv[3];
    if (strcmp(mode, "queue") !=0 && strcmp(mode, "sigrt") !=0 && strcmp(mode, "kill") !=0){
        printf("wrong mode\n");
        return 1;
    }

    printf("\n\nSender here! My PID: %d\n", getpid());
    printf("Catcher PID: %d\n", catcher_PID);
    printf("Number of signals to send: %d\n", to_send);
    printf("Mode: %s\n", mode);


    // SIGUSR1
    struct sigaction act_SIGUSR1;
    sigemptyset(&act_SIGUSR1.sa_mask);
    act_SIGUSR1.sa_flags = SA_SIGINFO;
    act_SIGUSR1.sa_sigaction = handler1;
    if (strcmp(mode,"sigrt") != 0)
        sigaction(SIGUSR1, &act_SIGUSR1, NULL);
    else
        sigaction(SIGRTMIN + 1, &act_SIGUSR1, NULL);

    //SIGUSR2
    struct sigaction act_SIGUSR2;
    sigemptyset(&act_SIGUSR2.sa_mask);
    act_SIGUSR2.sa_handler = handler2;
    if (strcmp(mode,"sigrt") != 0)
        sigaction(SIGUSR2, &act_SIGUSR2, NULL);
    else
        sigaction(SIGRTMIN + 2, &act_SIGUSR2, NULL);

    printf("\nSender going to send %d SIGUSR1 signals ......\n", to_send);
    for (int i = 0; i < to_send; i++) {
        printf("Sending SIGUSR1 (%d)...\n", i + 1);
        if (strcmp(mode, "kill") == 0) {
            kill(catcher_PID, SIGUSR1);

        }

        else if (strcmp(mode, "queue") == 0) {
            union sigval value;
            sigqueue(catcher_PID, SIGUSR1, value);
        }

        else {
            kill(catcher_PID, SIGRTMIN + 1);
        }
    }


    printf("Sender sent all [%d] SIGUSR1 signals.\n\n", to_send);

    // send SIGUSR2

    if (strcmp(mode, "kill") == 0) {
        kill(catcher_PID, SIGUSR2);
    }

    else if (strcmp(mode, "queue") == 0) {
        union sigval val;
        val.sival_int = 0;
        sigqueue(catcher_PID, SIGUSR2, val);
    }

    else{
        kill(catcher_PID, SIGRTMIN + 2);
    }


    while(1){}

}