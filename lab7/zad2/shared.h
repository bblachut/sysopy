#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>


#define OVEN_SHM "shm_oven"
#define TABLE_SHM "shm_table"

#define OVEN_SEMAPHORE "/OVEN_SEMAPHORE"
#define OVEN_TAKE_SEMAPHORE "/OVEN_TAKE_SEMAPHORE"
#define OVEN_PUT_SEMAPHORE "/OVEN_PUT_SEMAPHORE"
#define TABLE_SEMAPHORE "/TABLE_SEMAPHORE"
#define TABLE_TAKE_SEMAPHORE "/TABLE_TAKE_SEMAPHORE"
#define TABLE_PUT_SEMAPHORE "/TABLE_PUT_SEMAPHORE"

#define PREPARE_TIME 1
#define BAKING_TIME 4
#define DELIVERY_TIME 4
#define RETURN_TIME 4

#define OVEN_SIZE 5
#define TABLE_SIZE 5

typedef struct {
    int arr[OVEN_SIZE];
    int pizzas;
    int id_to_put;
    int id_to_take;
}oven;

typedef struct {
    int arr[TABLE_SIZE];
    int pizzas;
    int id_to_put;
    int id_to_take;
}table;

union semun {
    int val;
    struct semid_ds *buf;
    short *array;
};

void lock(sem_t* sem){
    if (sem_wait(sem) == -1){
        printf("Locking semaphore\n");
        exit(1);
    }
}

void unlock(sem_t* sem) {
    if (sem_post(sem) == -1){
        printf("Unlocking semaphore\n");
        exit(1);
    }
}

char * timestamp() {
    time_t now = time(NULL);
    char * time = asctime(gmtime(&now));
    time[strlen(time)-1] = '\0';
    return time;
}

sem_t* get_sem(char* name){
    // connect to an already existing semaphore
    sem_t* sem;
    if((sem = sem_open(name, O_RDWR)) == SEM_FAILED){
        printf("Connecting to semaphore %s!\n", name);
        printf("%s\n", strerror(errno));
        exit(1);
    }
    return sem;
}

int get_shm_oven_fd(){
    int shm_oven_fd;
    if ((shm_oven_fd = shm_open(OVEN_SHM, O_RDWR , 0666))== -1){
        printf("Getting oven id!\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    return shm_oven_fd;
}


int get_shm_table_fd(){
    int shm_table_fd;
    if ((shm_table_fd = shm_open(TABLE_SHM, O_RDWR , 0666)) == -1){
        printf("Getting table id!\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    return shm_table_fd;
}


#endif
