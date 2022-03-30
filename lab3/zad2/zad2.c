#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>

void timer_start(void);
void timer_check(void);
static struct tms g_tms;
static clock_t g_time;


void count(int id, double start, double end, double width);

double sum_files(int n);

int main(int argc, char** argv){
    if (argc != 3){
        return -1;
    }
    double width = atof(argv[1]);
    int n = atoi(argv[2]);
    pid_t pid;
    double start = 0;
    double width_per_process = 1.0/(double)n;
    double end = width_per_process;

    timer_start();
    for (int i = 1; i <= n; i++){
        pid = fork();
        if (pid == 0){
            count(i, start, end, width);
            exit(0);
        }
        start += width_per_process;
        end += width_per_process;
    }
    while(wait(NULL) > 0);

    double sum = sum_files(n);

    timer_check();

    printf("%f\n", sum);
    return 0;
}

double fun(double x){
    return 4/(pow(x, 2.0)+1);
}


void count(int id, double start, double end, double width){
    double result = 0.0;
    double internal_start = start;
    double internal_end = start+width;
    while (internal_end < (end-width/100.0)){
        result += fun((internal_start+internal_end)/2.0)*width;
        internal_start += width;
        internal_end += width;
    }
    char* filename = calloc(sizeof(char), 32);
    sprintf(filename, "w%d.txt", id);
    FILE* fp = fopen(filename, "w+");
    fprintf(fp, "%f", result);
    fclose(fp);
    free(filename);
}

double sum_files(int n){
    char* filename = calloc(sizeof(char), 32);
    char* result = calloc(sizeof(char), 32);
    double res = 0.0;
    for(int i = 1; i <= n; i++){
        sprintf(filename, "w%d.txt", i);
        FILE* fp = fopen(filename, "r");
        fread(result, sizeof (char), 32, fp);
        res += atof(result);
        fclose(fp);
        remove(filename);
    }

    free(filename);
    free(result);

    return res;
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
