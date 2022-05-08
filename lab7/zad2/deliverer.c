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

    sem_t* table_sem = get_sem(TABLE_SEMAPHORE);
    sem_t* table_put_sem = get_sem(TABLE_PUT_SEMAPHORE);
    sem_t* table_take_sem = get_sem(TABLE_TAKE_SEMAPHORE);
    int shm_table_id = get_shm_table_fd();

    table *t = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, shm_table_id, 0);

    srand(getpid());

    while (1) {

        // taking from table
        lock(table_take_sem);
        lock(table_sem);
        int type = take_from_table(t);
        unlock(table_put_sem);
        printf("[D]  (pid: %d timestamp: %s)  ->   Zabrałem pizze: %d. Liczba pizz na stole: %d.\n", getpid(),
               timestamp(), type, t->pizzas);
        unlock(table_sem);


        // delivery
        sleep(DELIVERY_TIME);
        printf("[D]  (pid: %d timestamp: %s)  ->   Dostarczyłem pizze: %d.\n", getpid(), timestamp(), type);

        //return
        sleep(RETURN_TIME);
        printf("[D]  (pid: %d timestamp: %s)  ->   Wróciłem.\n", getpid(), timestamp());

    }
}
