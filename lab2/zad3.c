#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <ftw.h>

int dirCounter = 0;
int regCounter = 0;
int chrCounter = 0;
int blkCounter = 0;
int lnkCounter = 0;
int fifoCounter = 0;
int sockCounter = 0;


int nftwfunc(const char *filename, const struct stat *sb,
             int fileflags, struct FTW *pfwt)
{
    char buf[1000];
    realpath(filename, buf);
    printf("This source is at %s.\n", buf);
    printf("File size:                %jd bytes\n", sb->st_size);
    printf("Link count:               %lu\n", sb->st_nlink);
    printf("Last file access:         %ld\n", sb->st_atime);
    printf("Last file modification:   %ld\n", sb->st_mtime);
    if (S_ISREG(sb->st_mode)){
        printf("File type:                regular file\n");
        regCounter = regCounter+1;
    } else if(S_ISDIR(sb->st_mode)){
        printf("File type:                directory\n");
        dirCounter = dirCounter+1;
    }else if(S_ISCHR(sb->st_mode)){
        printf("File type:                character device\n");
        chrCounter = chrCounter+1;
    }else if(S_ISBLK(sb->st_mode)){
        printf("File type:                block device\n");
        blkCounter = blkCounter+1;
    }else if(S_ISLNK(sb->st_mode)){
        printf("File type:                symbolic link\n");
        lnkCounter = lnkCounter+1;
    }else if(S_ISFIFO(sb->st_mode)){
        printf("File type:                pipe\n");
        fifoCounter = fifoCounter+1;
//            }else if(S_ISSOCK(sb.st_mode)){
//                printf("File type:                socket\n"); // some error idk why not working
//                sockCounter = sockCounter+1;
    }else{
        printf("File type:                unknown\n");
    }
    printf("\n\n");

    return 0;
}

void withNftw(char* path){
     int fd_limit = 1000;
     int flags = FTW_PHYS | FTW_CHDIR | FTW_MOUNT;
     nftw(path, nftwfunc, fd_limit, flags);


}

void checkDir(char* path){
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    char filePath[1000];
    if (d) {
        dir = readdir(d);//not showing current folder an one above
        dir = readdir(d);
        while ((dir = readdir(d)) != NULL) {
            strcpy(filePath, path);
            strcat(filePath, "/");
            strcat(filePath, dir->d_name);
            struct stat sb;
            if (lstat(filePath, &sb) == -1) {
                perror("lstat");
                exit(EXIT_FAILURE);
            }
            char buf[1000];
            realpath(filePath, buf);
            printf("This source is at %s.\n", buf);
            printf("File size:                %jd bytes\n", sb.st_size);
            printf("Link count:               %lu\n", sb.st_nlink);
            printf("Last file access:         %ld\n", sb.st_atime);
            printf("Last file modification:   %ld\n", sb.st_mtime);
            if (S_ISREG(sb.st_mode)){
                printf("File type:                regular file\n");
                regCounter = regCounter+1;
            } else if(S_ISDIR(sb.st_mode)){
                printf("File type:                directory\n");
                dirCounter = dirCounter+1;
            }else if(S_ISCHR(sb.st_mode)){
                printf("File type:                character device\n");
                chrCounter = chrCounter+1;
            }else if(S_ISBLK(sb.st_mode)){
                printf("File type:                block device\n");
                blkCounter = blkCounter+1;
            }else if(S_ISLNK(sb.st_mode)){
                printf("File type:                symbolic link\n");
                lnkCounter = lnkCounter+1;
            }else if(S_ISFIFO(sb.st_mode)){
                printf("File type:                pipe\n");
                fifoCounter = fifoCounter+1;
//            }else if(S_ISSOCK(sb.st_mode)){
//                printf("File type:                socket\n"); // some error idk why not working
//                sockCounter = sockCounter+1;
            }else{
                printf("File type:                unknown\n");
            }
            printf("\n\n");

            if (S_ISDIR(sb.st_mode)){
                checkDir(filePath);
            }
        }
        closedir(d);
    }
}

int main(int argc, char* argv[]) {

    withNftw(argv[1]);

//    checkDir(argv[1]);

    printf("dirCounter %d, regCounter %d, chrCounter %d, blkCounter %d, lnkCounter %d, fifoCounter %d, sockCounter %d\n",
           dirCounter, regCounter, chrCounter, blkCounter, lnkCounter, fifoCounter, sockCounter);
    return 0;
}