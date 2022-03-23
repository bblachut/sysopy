#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/times.h>

void timer_start(void);
void timer_check(void);
static struct tms g_tms;
static clock_t g_time;

void withRead(char *argv[]){
    long charCounter = 0;
    long lineCounter = 0;
    short was_read = 0;
    char* input = argv[2];
    char character = argv[1][0];
    char* curr_char = malloc(sizeof(char));
    int fd = open(input, O_RDONLY);
    int bytes_read = read(fd, curr_char, 1);
    while (bytes_read == 1){
        if (*curr_char == '\n'){
            was_read = 0;
        }
        if (*curr_char == character){
            charCounter++;
            if (!was_read){
                lineCounter++;
                was_read = 1;
            }
        }
        bytes_read = read(fd, curr_char, 1);
    }
    printf("%ld %c in %ld lines\n", charCounter, character, lineCounter);
    close(fd);
    free(curr_char);
}

void withFRead(char *argv[]){
    long charCounter = 0;
    long lineCounter = 0;
    short was_read = 0;
    char* input = argv[2];
    char character = argv[1][0];
    char* curr_char = malloc(sizeof(char));
    FILE *fp = fopen(input, "r");
    int bytes_read = fread(curr_char, sizeof(char), 1, fp);
    while (bytes_read == 1){
        if (*curr_char == '\n'){
            was_read = 0;
        }
        if (*curr_char == character){
            charCounter++;
            if (!was_read){
                lineCounter++;
                was_read = 1;
            }
        }
        bytes_read = fread(curr_char, sizeof(char), 1, fp);
    }
    printf("%ld %c in %ld lines\n", charCounter, character, lineCounter);
    free(curr_char);
}

int main(int argc, char *argv[]){
    if (argc != 3){
        exit(1);
    }
    timer_start();
//    withFRead(argv);
    withRead(argv);
    timer_check();
    return 0;
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

