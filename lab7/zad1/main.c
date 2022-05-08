#include "shared.h"

int shm_oven_id, shm_table_id, sem_id;

void create_semaphores() {
    key_t key;
    if ((key = ftok(getenv("HOME"), ID)) == -1) {
        printf("Generating key semaphore\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    // inaczej nie działa xD
    key++;

    if ((sem_id = semget(key, 6, 0666 | IPC_CREAT)) == -1) {
        printf("Creating semaphore set\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    union semun basic;
    union semun put_o;
    union semun put_t;
    union semun take;
    basic.val = 1;
    put_o.val = OVEN_SIZE;
    put_t.val = TABLE_SIZE;
    take.val = 0;
    if (semctl(sem_id, OVEN_SEMAPHORE, SETVAL, basic) == -1 || semctl(sem_id, OVEN_TAKE_SEMAPHORE, SETVAL, take) == -1 ||
    semctl(sem_id, OVEN_PUT_SEMAPHORE, SETVAL, put_o) == -1 || semctl(sem_id, TABLE_SEMAPHORE, SETVAL, basic) == -1 ||
    semctl(sem_id, TABLE_TAKE_SEMAPHORE, SETVAL, take) == -1 || semctl(sem_id, TABLE_PUT_SEMAPHORE, SETVAL, put_t) == -1) {
        printf("Setting table semaphore value\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }
}


int create_shm() {
    key_t key_o, key_t;
    if((key_o = ftok(OVEN_PATH, OVEN_ID)) == -1) {
        printf("Generating key shm\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    if((key_t = ftok(TABLE_PATH, TABLE_ID)) == -1) {
        printf("Generating key\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    if ((shm_oven_id = shmget(key_o, sizeof(oven), IPC_CREAT | 0666)) == -1){
        printf("Error while creating shared memory segment\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    if ((shm_table_id = shmget(key_t, sizeof(table), IPC_CREAT | 0666)) == -1){
        printf("Error while creating shared memory segment\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    oven *o = shmat(shm_oven_id, NULL, 0);
    o->pizzas = 0;
    o->id_to_put = 0;
    o->id_to_take = 0;
    for(int i = 0; i < OVEN_SIZE; i++) {
        o->arr[i] = -1;
    }

    table *t = shmat(shm_table_id, NULL, 0);
    t->pizzas = 0;
    t->id_to_put = 0;
    t->id_to_take = 0;
    for(int i = 0; i < TABLE_SIZE; i++) {
        t->arr[i] = -1;
    }

    return 0;
}


void handler(int signum){
    semctl(sem_id, 0, IPC_RMID, NULL);
    shmctl(shm_oven_id, IPC_RMID, NULL);
    shmctl(shm_table_id, IPC_RMID, NULL);
}


int main(int argc, char **argv) {
    if(argc != 3) {
        puts("WRONG NUMBER OF ARGUMENTS\n");
        return 1;
    }

    signal(SIGINT, handler);


    int cooks = atoi(argv[1]);
    int deliverers = atoi(argv[2]);
    printf("STARTING PIZZERIA with %d cooks and %d deliverers\n", cooks, deliverers);

    create_semaphores();
    create_shm();

    for (int i = 0; i < cooks; i++){
        pid_t pid = fork();
        if (pid == 0){
            execl("./cook", "./cook", NULL);
            printf("Return not expected, execl() error\n");

        }
    }

    for (int i = 0; i < deliverers; i++){
        pid_t pid = fork();
        if (pid == 0){
            execl("./deliverer", "./deliverer", NULL);
            printf("Return not expected, execl() error\n");

        }
    }

    for(int i = 0; i < cooks + deliverers; i++)
        wait(NULL);


    return 0;
}