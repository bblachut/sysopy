#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wcLib.h"

int findFirstFreeID(wcMainTable * array);
char* readFile(FILE* file);

wcMainTable wc_createMainTable(size_t nEntries){
    wcMainTable newTable;
    newTable.size = nEntries;
    newTable.data = calloc(nEntries, sizeof(*(newTable.data)));
    return newTable;
}

void removeBlock(wcMainTable * array, int id){
    if (id >= array->size)
        return;

    free(array->data[id]);
}

int wc_addBlock(wcMainTable*array, const char* path){
    const int id = findFirstFreeID(array);

    FILE* fp;
    char* tmpName = "/tmp.txt";
    fp = fopen(tmpName, "w+");
    const size_t additional_commnad_buffer_size = 9;
    char* command = calloc(sizeof(*command), additional_commnad_buffer_size + strlen(path) + strlen(tmpName));
    sprintf(command,"wc %s > %s", path, tmpName);
    system(command);

    array->data[id] = readFile(fp);
    fclose(fp);
    return id;
}

int findFirstFreeID(wcMainTable * array) {
    for (int i = 0; i < array->size; ++i)
        if(array->data[i] == NULL)
            return i;
    return array->size;
}

char* readFile(FILE* file) {
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    rewind(file);

    char *str = calloc(sizeof(*str), fsize + 1);
    fread(str, fsize, 1, file);

    return str;
}