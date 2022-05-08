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
    int sem_id = get_sem_id();
    int shm_oven_id = get_shm_oven_id();
    int shm_table_id = get_shm_table_id();

    oven* o = shmat(shm_oven_id, NULL, 0);
    table* t = shmat(shm_table_id, NULL, 0);

    srand(getpid());

    while(1){

        // preparing
        int type = rand() % 10;
        printf("[C]  (pid: %d timestamp: %s)  ->   Przygotowuje pizze: %d\n", getpid(), timestamp(), type);
        sleep(PREPARE_TIME);

        // wait for free place in oven
        lock(sem_id, OVEN_PUT_SEMAPHORE);
        // placing in oven
        lock(sem_id, OVEN_SEMAPHORE);
        place_in_oven(o, type);
        printf("[C]  (pid: %d timestamp: %s)  ->   Dodałem pizze: %d. Liczba pizz w piecu: %d.\n", getpid(), timestamp(), type, o->pizzas);
        unlock(sem_id, OVEN_SEMAPHORE);
        unlock(sem_id, OVEN_TAKE_SEMAPHORE);



        // baking
        sleep(BAKING_TIME);


        // taking out
        lock(sem_id, OVEN_TAKE_SEMAPHORE);
        lock(sem_id, OVEN_SEMAPHORE);
        type = take_out_pizza(o);
        printf("[C]  (pid: %d timestamp: %s)  ->   Wyjalem pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", getpid(), timestamp(), type, o->pizzas, t->pizzas);
        unlock(sem_id, OVEN_SEMAPHORE);
        unlock(sem_id, OVEN_PUT_SEMAPHORE);


        // placing on the table
        lock(sem_id, TABLE_PUT_SEMAPHORE);
        lock(sem_id, TABLE_SEMAPHORE);
        place_on_table(t, type);
        printf("[C]  (pid: %d timestamp: %s)  ->   Umieszczam pizze na stole: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", getpid(), timestamp(), type, o->pizzas, t->pizzas);
        unlock(sem_id, TABLE_SEMAPHORE);
        unlock(sem_id, TABLE_TAKE_SEMAPHORE);
    }
}