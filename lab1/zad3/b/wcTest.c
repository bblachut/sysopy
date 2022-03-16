#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>

#include "wcLib.h"

char* read_command_arguments(void);

void timer_start(void);
void timer_check(void);
static struct tms g_tms;
static clock_t g_time;

int main(){
    wcMainTable *array = malloc(sizeof(*array));

    while (1){
        char input[256];
        printf(">> ");
        scanf("%s", input);
        char* args = read_command_arguments();
        if (strcmp(input, "create_table") == 0) {
            int size = atoi(args);
            free(array->data);
            timer_start();
            wcMainTable newArray = wc_createMainTable(size);
            array->size = newArray.size;
            array->data = newArray.data;
            timer_check();
        } else if (strcmp(input, "wc_files") == 0) {
            char delim[] = " ";

            char *ptr = strtok(args, delim);

            timer_start();
            while(ptr != NULL)
            {
                int id;
                id = wc_addBlock(array, ptr);
                printf("Saved at position %d\n", id);
                ptr = strtok(NULL, delim);
            }
            timer_check();

        } else if (strcmp(input, "remove_block") == 0) {
            int id = atoi(args);
            timer_start();
            wc_removeBlock(array, id);
            timer_check();
        } else if (strcmp(input, "q") == 0) {
            return 0;
        }else {
            printf("Wrong command");
        }
    }
}

char* read_command_arguments() {
    char* args = calloc(sizeof(*args), 256);
    fgets(args, 256, stdin);
    args[strlen(args) - 1] = '\0';
    return args;
}

void timer_start() {
    g_time = times(&g_tms);
}

double get_time_diff(clock_t start, clock_t end) {
    return (double)(end - start) / (double)sysconf(_SC_CLK_TCK);
}

void timer_check() {
    struct tms tms_now;
    clock_t time_end = times(&tms_now);

    printf("Time Real: %f\n", get_time_diff(g_time, time_end));
    printf("Time User: %f\n", get_time_diff(g_tms.tms_utime, tms_now.tms_utime));
    printf("Time Sys: %f\n", get_time_diff(g_tms.tms_stime, tms_now.tms_stime));
}