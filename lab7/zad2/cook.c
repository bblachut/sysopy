#include "shared.h"


void place_in_oven(oven* o, int type){
    o->arr[o->id_to_put] = type;
    o->id_to_put++;
    o->id_to_put = o->id_to_put % OVEN_SIZE;
    o->pizzas++;
}

int take_out_pizza(oven* o){
    int type = o->arr[o->id_to_take];
    o->arr[o->id_to_take] = -1;
    o->id_to_take++;
    o->id_to_take = o->id_to_take % OVEN_SIZE;
    o->pizzas--;
    return type;
}


void place_on_table(table* t, int type){
    t->arr[t->id_to_put] = type;
    t->id_to_put++;
    t->id_to_put = t->id_to_put % TABLE_SIZE;
    t->pizzas++;
}

int main() {
    sem_t* oven_sem = get_sem(OVEN_SEMAPHORE);
    sem_t* oven_put_sem = get_sem(OVEN_PUT_SEMAPHORE);
    sem_t* oven_take_sem = get_sem(OVEN_TAKE_SEMAPHORE);
    sem_t* table_sem = get_sem(TABLE_SEMAPHORE);
    sem_t* table_put_sem = get_sem(TABLE_PUT_SEMAPHORE);
    sem_t* table_take_sem = get_sem(TABLE_TAKE_SEMAPHORE);

    int shm_oven = get_shm_oven_fd();
    int shm_table = get_shm_table_fd();

    oven* o = mmap(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, shm_oven, 0);
    table* t = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, shm_table, 0);

    srand(getpid());

    while(1){

        // preparing
        int type = rand() % 10;
        printf("[C]  (pid: %d timestamp: %s)  ->   Przygotowuje pizze: %d\n", getpid(), timestamp(), type);
        sleep(PREPARE_TIME);

        // wait for free place in oven
        lock(oven_put_sem);
        // placing in oven
        lock(oven_sem);
        place_in_oven(o, type);
        printf("[C]  (pid: %d timestamp: %s)  ->   Dodałem pizze: %d. Liczba pizz w piecu: %d.\n", getpid(), timestamp(), type, o->pizzas);
        unlock(oven_sem);
        unlock(oven_take_sem);



        // baking
        sleep(BAKING_TIME);


        // taking out
        lock(oven_take_sem);
        lock(oven_sem);
        type = take_out_pizza(o);
        printf("[C]  (pid: %d timestamp: %s)  ->   Wyjalem pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", getpid(), timestamp(), type, o->pizzas, t->pizzas);
        unlock(oven_sem);
        unlock(oven_put_sem);


        // placing on the table
        lock(table_put_sem);
        lock(table_sem);
        place_on_table(t, type);
        printf("[C]  (pid: %d timestamp: %s)  ->   Umieszczam pizze na stole: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", getpid(), timestamp(), type, o->pizzas, t->pizzas);
        unlock(table_sem);
        unlock(table_take_sem);
    }
}