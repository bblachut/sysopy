#include <stdio.h>
#include <signal.h>
#include <sys/times.h>
#include <unistd.h>

void siginfo();
void nodefer();
void resethand();

int main() {
    siginfo();
    nodefer();
    resethand();
}

void sig_handler(int signum, siginfo_t *info, void *ctx) {
    printf("received signal %d\n", info->si_signo);
    printf("status: %d\n", info->si_status);
    printf("sender PID: %d\n", info->si_pid);
    printf("sender UID: %d\n", info->si_uid);
    printf("utime: %ld\n", info->si_utime);
}

void siginfo() {
    printf("\n\n###siginfo###\n");
    struct sigaction act;
    act.sa_sigaction = sig_handler;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);

    sigaction(SIGUSR1, &act, NULL);

    raise(SIGUSR1);
}

void nodefer(){
    printf("\n\n###nodefer###\n");

    struct sigaction act;
    act.sa_sigaction = sig_handler;
    act.sa_flags = SA_NODEFER;
    kill(getpid(), SIGUSR1);
}

void resethand(){
    printf("\n\n###resethend###\n");
    struct sigaction act;
    act.sa_sigaction = sig_handler;
    act.sa_flags = SA_RESETHAND;
    kill(getpid(), SIGUSR1);
}

