#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char** argv){
    if (argc != 2){
        return -1;
    }
    int n = atoi(argv[1]);

    pid_t child_pid;
    for (int i = 0; i < n; i++){
        child_pid = fork();
        if (child_pid == 0){
            printf("Child PID = %d\n", (int)getpid());
            break;
        }
    }
}