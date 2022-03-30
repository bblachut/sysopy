#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ftw.h>
#include <unistd.h>
#include <sys/wait.h>

int search_str(char* path, char* str);
int is_txt(char* filename);
void checkDir(char* path, int chars_to_cut, int depth, int max_depth, char* str);


int main(int argc, char** argv){
    if(argc != 4){
        return -1;
    }
    int max_depth = atoi(argv[3]);
    char* str = argv[2];
    char* path = argv[1];
    int chars_to_cut = strlen(path);
    checkDir(path, chars_to_cut, 0, max_depth, str);
    while(wait(NULL) > 0);
    return 0;
}

int search_str(char* path, char* str){
    FILE* fp = fopen(path, "r");
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);

    char *file = calloc(sizeof(*file), fsize + 1);
    fread(file, fsize, 1, fp);
    fclose(fp);
    if (strstr(file, str) == NULL){
        return 0;
    } else return 1;
}

void checkDir(char* path, int chars_to_cut, int depth, int max_depth, char* str){
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    char file_path[1000];
    if (d) {
        dir = readdir(d);//not showing current folder an one above
        dir = readdir(d);
        while ((dir = readdir(d)) != NULL) {
            strcpy(file_path, path);
            strcat(file_path, "/");
            strcat(file_path, dir->d_name);
            struct stat sb;
            if (lstat(file_path, &sb) == -1) {
                perror("lstat");
                exit(EXIT_FAILURE);
            }
            if (S_ISREG(sb.st_mode)){
                if(is_txt(file_path) && search_str(file_path, str)){
                    const char* relative_path = &(file_path[chars_to_cut]);
                    printf("path = %s  pid = %d \n", relative_path, (int)getpid());
                }
            } else if(S_ISDIR(sb.st_mode)) {
                pid_t pid = fork();
                if (pid == 0){
                    checkDir(file_path, chars_to_cut, depth, max_depth, str);
                    return;
                }
            }
        }
    }
    closedir(d);
}
int is_txt(char* filename){
    return strcmp(&filename[strlen(filename) - 4], ".txt") == 0;
}