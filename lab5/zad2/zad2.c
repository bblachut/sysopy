#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_output(FILE* file){
    char line[256];
    while(fgets(line, 256, file) != NULL){
        printf("%s\n", line);
    }
}

void print_ordered_by(char* mode){
    FILE* file;
    char* command;

    if (strcmp(mode,"date") == 0) {
        command = "echo | mail -f | tail +2 | head -n -1 | tac";
    }
    else if (strcmp(mode, "sender") == 0){
        command = "echo | mail -f | tail +2 | head -n -1 | sort -k 2";
    }
    else{
        printf("Choose date or sender.\n");
        exit(1);
    }

    file = popen(command, "r");

    printf("\nSORTED EMAILS: ");
    print_output(file);
}

void send_mail(char* mail, char* title, char* contents){
    char * command = malloc(10000);
    sprintf(command, "echo %s | mail -s %s %s", contents, title, mail);
    printf("%s\n", command);
    FILE *popened = popen(command, "r");
    pclose(popened);
}

int main(int argc, char* argv[]) {

    if (argc != 2 && argc != 4) {
        printf("Wrong number of arguments!");
        exit(1);
    }

    if (argc == 2) {
        print_ordered_by(argv[1]);
    } else{
        char* address = argv[1];
        char* subject = argv[2];
        char* content = argv[3];

        send_mail(address, subject, content);
    }
}