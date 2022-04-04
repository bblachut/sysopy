#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


void test_pending();
void test_mask();
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
    printf("-------Testing pending-------\n");

    sigset_t base_mask;
    sigset_t waiting_mask;

    sigemptyset (&base_mask);
    sigaddset (&base_mask, SIGUSR1);
    sigprocmask(SIG_SETMASK, &base_mask, NULL);

    raise(SIGUSR1);

    sigpending (&waiting_mask);

    printf("%s\n", sigismember (&waiting_mask, SIGUSR1) ? "Pending in parent" : "Not Pending in parent");

    char *args[] = { "./to_exec", "pending", NULL };
    execvp(args[0], args);
}
void test_mask(){
    printf("-------Testing mask-------\n");

    sigset_t new_mask;
    sigset_t old_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

    if(sigismember(&new_mask, SIGUSR1)) {
        printf("%d Blocked\n", SIGUSR1);
    } else {
        printf("%d Not blocked\n", SIGUSR1);
    }

    signal(SIGUSR1, handler);

    printf("Raising in parent\n");
    raise(SIGUSR1);

    sigset_t waiting_mask;
    sigpending(&waiting_mask);
    if(sigismember(&waiting_mask, SIGUSR1)) {
        printf("Signal masked, nothing happened in parent\n");
    }

    printf("Unmasking signal and handling\n");
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigprocmask(SIG_UNBLOCK, &new_mask, &old_mask);

    //masking again
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

    char *args[] = { "./to_exec", "mask", NULL };
    execvp(args[0], args);
}

void test_ignore(){
    printf("-------Testing ignore-------\n");
    signal(SIGUSR1, SIG_IGN);

    printf("Parent raising signal...\n");
    raise(SIGUSR1);
    printf("Parent ignored signal\n");

    char *args[] = { "./to_exec", "ignore", NULL };
    execvp(args[0],args);
}
