#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

int caught = 0;
char* mode;

void handler1(int signum){
    caught++;
}
void handler2(int sig, siginfo_t *info, void *ucontext);

int main(int argc, char* argv[]) {

    if (argc != 2){
        printf("Wrong number of arguments\n");
        return -1;
    }

    printf("\n\nCatcher PID: %d\n", getpid());

    mode = argv[1];
    if (strcmp(mode, "queue") !=0 && strcmp(mode, "sigrt") !=0 && strcmp(mode, "kill") !=0){
        printf("wrong mode\n");
        return 1;
    }
    printf("Mode: %s\n", mode);

    //SIGUSR1
    struct sigaction act_SIGUSR1;
    sigemptyset(&act_SIGUSR1.sa_mask);
    act_SIGUSR1.sa_handler = handler1;
    if (strcmp(mode,"sigrt") != 0)
        sigaction(SIGUSR1, &act_SIGUSR1, NULL);
    else
        sigaction(SIGRTMIN + 1, &act_SIGUSR1, NULL);


    //SIGUSR2
    struct sigaction act_SIGUSR2;
    sigemptyset(&act_SIGUSR2.sa_mask);
    act_SIGUSR2.sa_flags = SA_SIGINFO;
    act_SIGUSR2.sa_sigaction  = handler2;
    if (strcmp(mode,"sigrt") != 0)
        sigaction(SIGUSR2, &act_SIGUSR2, NULL);
    else
        sigaction(SIGRTMIN + 2, &act_SIGUSR2, NULL);


    sigset_t mask;
    sigfillset(&mask);
    if(strcmp(mode, "sigrt") == 0){
        sigdelset(&mask, SIGRTMIN+1);
        sigdelset(&mask, SIGRTMIN+2);
    } else{
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGUSR2);
    }
    sigprocmask(SIG_SETMASK, &mask, NULL);


    while(1){
        sigsuspend(&mask);
    }

}

void handler2(int sig, siginfo_t *info, void *ucontext){
    printf("--------------------------- ALL SIGNALS SENT -------------------------------------\n\n");
    printf("Catcher received SIGUSR2, now is going to send back [%d] received SIGUSR1 signals.\n", caught);

    // get sender PID from siginfo_t
    pid_t sender_PID = info->si_pid;

    // SIGUSR2 handled - send back SIGUSR1 (caught times)
    for (int i = 0; i < caught; i++){
        if (strcmp(mode,"kill") == 0){
            kill(sender_PID, SIGUSR1);
        }
        else if (strcmp(mode,"queue") == 0){
            union sigval val;
            // send number of sent back signal
            val.sival_int = i;
            sigqueue(sender_PID, SIGUSR1, val);
        }
        else {
            kill(sender_PID, SIGRTMIN+1);
        }
    }

    // SIGUSR2 send back (caught times) - send SIGUSR2
    if (strcmp(mode,"kill") == 0){
        kill(sender_PID, SIGUSR2);
    }else if (strcmp(mode,"queue") == 0){
        union sigval val;
        val.sival_int = 0;
        sigqueue(sender_PID, SIGUSR2, val);
    }else{
        kill(sender_PID, SIGRTMIN+2);
    }

    exit(0);
}