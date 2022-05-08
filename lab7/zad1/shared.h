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

#define OVEN_ID 'O'
#define TABLE_ID 'T'
#define ID 'I'

#define OVEN_PATH "./cook"
#define TABLE_PATH "./deliverer"

#define OVEN_SEMAPHORE 0
#define OVEN_TAKE_SEMAPHORE 1
#define OVEN_PUT_SEMAPHORE 2
#define TABLE_SEMAPHORE 3
#define TABLE_TAKE_SEMAPHORE 4
#define TABLE_PUT_SEMAPHORE 5

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

void lock(int semaphore_id, int semaphore_num) {
    struct sembuf s = {.sem_num = semaphore_num, .sem_op = -1};
    if (semop(semaphore_id, &s, 1) == -1) {
        printf("Locking semaphore\n");
        exit(1);
    }
}

void unlock(int semaphore_id, int semaphore_num) {
    struct sembuf s = {.sem_num = semaphore_num, .sem_op = 1};
    if (semop(semaphore_id, &s, 1) == -1) {
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

int get_sem_id(){
    key_t key = ftok(getenv("HOME"), ID)+1;
    int sem_id;
    if((sem_id = semget(key, 6, 0)) == -1){
        printf("Error while connecting to semaphore set!\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    return sem_id;
}

int get_shm_oven_id(){
    key_t key_o = ftok(OVEN_PATH, OVEN_ID);
    int shm_oven_id;
    if ((shm_oven_id = shmget(key_o, sizeof(oven), 0)) == -1){
        printf("Getting oven id!\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    return shm_oven_id;
}


int get_shm_table_id(){
    key_t key_t = ftok(TABLE_PATH, TABLE_ID);
    int shm_table_id;
    if ((shm_table_id = shmget(key_t, sizeof(table), 0)) == -1){
        printf("Getting table id!\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    return shm_table_id;
}

#endif
