#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define MAX_ARGS 20  // in line
#define MAX_LINES 10


char** parse(char* line){

    char** commands = (char**)calloc(MAX_ARGS, sizeof(char*));

    char* seq = strtok(line, "=");

    int ctr = 0;
    while ((seq = strtok(NULL, "|")) != NULL){
        commands[ctr++] = seq;
    }

    return commands;
}


char** get_program_args(char* command, char* path){

    char** args = (char**)calloc(256, sizeof(char*));

    char* arg = strtok(command, " ");
    strcpy(path, arg);  // program name

    int ctr = 0;
    args[ctr++] = arg;  // program name
    while ((arg = strtok(NULL, " ")) != NULL){
        args[ctr++] = arg;
    }
    args[ctr] = NULL;

    return args;
}


int get_line_num(char* line, int i){
    // format: commandsX,  0 <= X <= 9
    if (i == 0) return line[8] - '0';
    else return line[9] - '0';
}


int* get_lines_to_execute(char* line){
    // format of line: commands0 | commands1 | ... | commandsN
    // where commandsX is Xth line of file, so we need to know X
    // which is idx of given line

    char** lines = (char**)calloc(MAX_ARGS, sizeof(char*));
    char* arg = strtok(line, "|");

    int ctr = 0;
    lines[ctr++] = arg;

    while ((arg = strtok(NULL, "|")) != NULL){
        lines[ctr++] = arg;
    }

    static int lines_num[MAX_ARGS];
    int i = 0;
    while(lines[i] != NULL) {
        lines_num[i] = get_line_num(lines[i], i);
        i++;
    }
    // end of arr
    lines_num[i] = -1;

    return lines_num;
}


void parse_execute(FILE* file){
    char** lines = (char**)calloc(MAX_LINES, sizeof(char*));   // commands_in_line: lines with "="
    char** commands =(char**)calloc(MAX_LINES*MAX_ARGS, sizeof(char*)) ;
    int commands_ctr;
    char** args;
    char* line = (char*)calloc(256, sizeof(char));

    int* lines_num;
    int line_ctr = 0;
    int lines_number;

    while(fgets(line, 256 * sizeof(char), file)) {
        printf("\n--------------------------------------------");
        printf("\nLINE: %s\n", line);

        if (strstr(line, "=")) {
            // if line contains "=" , let's add it to array of lines
            char *line_copy = (char *) calloc(256, sizeof(char));
            strcpy(line_copy, line);
            lines[line_ctr++] = line_copy;
        }

        else {
            // get lines number to execute it (from "lines" arr)
            commands_ctr = 0;
            lines_num = get_lines_to_execute(line);
            printf("lines to execute: \n");
            lines_number = 0;
            while (lines_num[lines_number] != -1) {
                printf("lin %d\n", lines_num[lines_number]);
                lines_number++;
            }

            // get all commands
            for (int i = 0; i < lines_number; i++) {
                // get commands_in_line from one line
                char** commands_in_line;
                commands_in_line = parse(lines[lines_num[i]]);

                // print parsed commands_in_line
                int m = 0;
                while (commands_in_line[m] != NULL) {
                    printf("com%d:  %s\n", commands_ctr + 1, commands_in_line[m]);
                    commands[commands_ctr] = commands_in_line[m];
                    commands_ctr++;
                    m++;
                }
                //cutting new line sign
                int x = 0;
                while (commands[commands_ctr-1][x] != '\n'){
                    x++;
                }
                commands[commands_ctr-1][x] = '\0';
            }

            //    Create a buffer for the pipes
            int fds[commands_ctr-1][2];
            for (int i = 0; i < commands_ctr-1; ++i) {
                pipe(fds[i]);
            }
            //Run all of the commands
            for (int i = 0; i < commands_ctr; ++i) {
                pid_t forked = fork();
                if(forked == 0){

            // Setup the correct outputs
                    if(i != 0){
                        dup2(fds[i-1][0], STDIN_FILENO);
                    }
                    if (i != commands_ctr-1){
                        dup2(fds[i][1], STDOUT_FILENO);
                    }

                    for (int j = 0; j < commands_ctr-1; ++j) {
                        close(fds[j][1]);
                        close(fds[j][0]);
                    }
//              Execute
                    char path[256];
                    args = get_program_args(commands[i], path);
                    int m = 0;
                    while (args[m] != NULL) {
//                        printf("arg%d: %s\n", m + 1, args[m]);
                        m++;
                    }

                    if (execvp(path, args) == -1) {
                        printf("ERROR in exec\n");
                        exit(1);
                    }
                    exit(0);
                }

            }
            for (int j = 0; j < commands_ctr-1; ++j) {
                close(fds[j][1]);
                close(fds[j][0]);
            }
            // wait for all the child processes to terminate
            int status = 0;
            pid_t wpid;
            while ((wpid = wait(&status)) != -1);
            printf("\nALL CHILDREN TERMINATED\n");
        }
    }

}

int main(int argc, char* argv[]){
    if (argc != 2){
        printf("Wrong number of arguments!\n");
        exit(1);
    }

    char* path = argv[1];
    FILE* commands = fopen(path, "r");

    if (commands == NULL){
        printf("Cannot open file\n");
        exit(1);
    }

    parse_execute(commands);

    fclose(commands);

}