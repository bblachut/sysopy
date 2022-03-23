#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/times.h>

void timer_start(void);
void timer_check(void);
static struct tms g_tms;
static clock_t g_time;


char* readFile(FILE* file) {
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    rewind(file);

    char *str = calloc(sizeof(*str), fsize + 1);
    fread(str, fsize, 1, file);

    return str;
}

void withRead(char* tmpName, char* output){
    int fp = open(tmpName, O_RDONLY);
    int fp2 = open(output, O_RDWR | O_CREAT);
    char buf[256];
    int read_chars;
    do {
        read_chars = read(fp, buf, 256*sizeof(char));
        write(fp2, buf, sizeof(char)*read_chars);
    } while (read_chars == 256);
    close(fp);
    close(fp2);
}

void  withFRead(FILE* fp, char* output){
    char* str = readFile(fp);
    fclose(fp);
    fp = fopen(output, "w+");
    fwrite(str, sizeof(char), strlen(str), fp);
    free(str);
}

int main(int argc, char *argv[]){
    char* input = malloc(sizeof(char)*256);
    char* output = malloc(sizeof(char)*256);
    if (argc != 3){
        printf("input file.txt >>");
        scanf("%s", input);
        printf("output file.txt >>");
        scanf("%s", output);
    } else{
        input = argv[1];
        printf("input is %s\n", input);
        output = argv[2];
        printf("output is %s\n", output);
    }

    FILE* fp;
    char* tmpName = "./tmp.txt";
    fp = fopen(tmpName, "w+");
    const size_t additional_commnad_buffer_size = 100;
    char* command = calloc(sizeof(*command), additional_commnad_buffer_size + strlen(input) + strlen(tmpName));
    sprintf(command,"grep -v -e '^[[:space:]]*$' %s > %s", input, tmpName);
    system(command);
    free(command);

    timer_start();
//    withRead(tmpName, output);
    withFRead(fp, output);
    timer_check();

    fclose(fp);
    remove("tmp.txt");

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



