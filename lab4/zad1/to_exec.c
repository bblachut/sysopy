#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>


void test_pending();
void test_mask();
void test_handler();
void test_ignore();


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Wrong number of arguments!");
        return -1;
    }
    if(strcmp(argv[1], "ignore") == 0) {
        test_ignore();
    } else if(strcmp(argv[1], "mask") == 0) {
        test_mask();
    } else if(strcmp(argv[1], "pending") == 0) {
        test_pending();
    } else {
        printf("wrong argument\n");
    }
    return 0;
}


void handler(int signum){
    printf("Received signal %d. PID: %d, PPID: %d\n", signum, getpid(), getppid());
}

void test_pending(){
    sigset_t waiting_mask;
    sigpending (&waiting_mask);
    printf("%s\n", sigismember (&waiting_mask, SIGUSR1) ? "Pending in child" : "Not Pending in child");
}
void test_mask(){
    sigset_t new_mask;
    sigset_t old_mask;
    sigemptyset(&new_mask);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

    if(sigismember(&new_mask, SIGUSR1)) {
        printf("%d Blocked\n", SIGUSR1);
    } else {
        printf("%d Not blocked\n", SIGUSR1);
    }

    printf("Raising in child\n");
    signal(SIGUSR1, handler);
    raise(SIGUSR1);

    sigset_t waiting_mask;
    sigpending(&waiting_mask);
    if(sigismember(&waiting_mask, SIGUSR1)) {
        printf("Signal masked, nothing happened in child\n");

    }
    printf("Unmasking signal and handling\n");
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigprocmask(SIG_UNBLOCK, &new_mask, &old_mask);
}

void test_ignore(){
    printf("Child raising signal...\n");
    raise(SIGUSR1);
    printf("Child ignored signal\n");
}
//
// Created by bogoj on 03.04.2022.
//

