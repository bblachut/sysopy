#include "shared.h"

int shm_oven_id, shm_table_id, sem_id;

sem_t* create_semaphore(char* name, int value){
    sem_t* new_sem = sem_open(name, O_CREAT, 0666, value);
    if (new_sem == SEM_FAILED){
        printf("Creating %s semaphore!\n", name);
        exit(1);
    }
    return new_sem;
}

void create_semaphores() {
   sem_t* oven_sem = create_semaphore(OVEN_SEMAPHORE, 1);
   sem_t* oven_put_sem = create_semaphore(OVEN_PUT_SEMAPHORE, OVEN_SIZE);
   sem_t* oven_take_sem = create_semaphore(OVEN_TAKE_SEMAPHORE, 0);
   sem_t* table_sem = create_semaphore(TABLE_SEMAPHORE, 1);
   sem_t* table_put_sem = create_semaphore(TABLE_PUT_SEMAPHORE, TABLE_SIZE);
   sem_t* table_take_sem = create_semaphore(TABLE_TAKE_SEMAPHORE, 0);
}


int create_shm() {
    int shm_oven, shm_table;

    if ((shm_oven = shm_open(OVEN_SHM, O_RDWR | O_CREAT, 0666)) == - 1) {
        printf("Creating shared memory\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    if ((shm_table = shm_open(TABLE_SHM, O_RDWR | O_CREAT, 0666)) == - 1) {
        printf("Creating shared memory\n");
        exit(1);
    }

    if (ftruncate(shm_oven, sizeof(oven)) == -1) {
        printf("Ftruncate\n");
        exit(1);
    }

    if (ftruncate(shm_table, sizeof(table)) == -1) {
        printf("Ftruncate\n");
        exit(1);
    }

    oven *o = mmap(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, shm_oven, 0);
    o->pizzas = 0;
    o->id_to_put = 0;
    o->id_to_take = 0;
    for(int i = 0; i < OVEN_SIZE; i++) {
        o->arr[i] = -1;
    }

    table* t = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, shm_table, 0);
    t->pizzas = 0;
    t->id_to_put = 0;
    t->id_to_take = 0;
    for(int i = 0; i < TABLE_SIZE; i++) {
        t->arr[i] = -1;
    }

    return 0;
}


void handler(int signum){
    sem_unlink(OVEN_SEMAPHORE);
    sem_unlink(OVEN_PUT_SEMAPHORE);
    sem_unlink(OVEN_TAKE_SEMAPHORE);
    sem_unlink(TABLE_SEMAPHORE);
    sem_unlink(TABLE_PUT_SEMAPHORE);
    sem_unlink(TABLE_TAKE_SEMAPHORE);
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