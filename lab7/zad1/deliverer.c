#include "shared.h"


int take_from_table(table * t){
    int type = t->arr[t->id_to_take];
    t->arr[t->id_to_take] = -1;
    t->id_to_take++;
    t->id_to_take = t->id_to_take % TABLE_SIZE;
    t->pizzas--;
    return type;
}

int main() {

    int sem_id = get_sem_id();
    int shm_table_id = get_shm_table_id();

    table *t = shmat(shm_table_id, NULL, 0);

    srand(getpid());

    while (1) {

        // taking from table
        lock(sem_id, TABLE_TAKE_SEMAPHORE);
        lock(sem_id, TABLE_SEMAPHORE);
        int type = take_from_table(t);
        unlock(sem_id, TABLE_PUT_SEMAPHORE);
        printf("[D]  (pid: %d timestamp: %s)  ->   Zabrałem pizze: %d. Liczba pizz na stole: %d.\n", getpid(),
               timestamp(), type, t->pizzas);
        unlock(sem_id, TABLE_SEMAPHORE);



        // delivery
        sleep(DELIVERY_TIME);
        printf("[D]  (pid: %d timestamp: %s)  ->   Dostarczyłem pizze: %d.\n", getpid(), timestamp(), type);

        //return
        sleep(RETURN_TIME);
        printf("[D]  (pid: %d timestamp: %s)  ->   Wróciłem.\n", getpid(), timestamp());

    }
}
