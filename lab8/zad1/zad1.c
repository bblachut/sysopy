#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

#define ROW_LEN 256

int** image;
int** negative_image;
int w, h;
int threads_num;


void load_image(char* filename){
    FILE* f = fopen(filename, "r");
    if (f == NULL){
        printf("Error while opening input file\n");
        exit(1);
    }


    char* buffer = calloc(ROW_LEN, sizeof(char));

    // skip P2
    fgets(buffer, ROW_LEN, f);

    // scan w and h
    fgets(buffer, ROW_LEN, f);
    sscanf(buffer, "%d %d\n", &w, &h);

    // skip 255
    fgets(buffer, ROW_LEN, f);


    // read image
    image = calloc(h, sizeof(int *));
    for (int i = 0; i < h; i++) {
        image[i] = calloc(w, sizeof(int));
    }

    int pixel;

    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            fscanf(f, "%d", &pixel);
            image[i][j] = pixel;
        }
    }

    fclose(f);
}

void* numbers_method(void* arg){
    struct timeval stop, start;
    gettimeofday(&start, NULL);

    int idx = *((int *) arg);


    int from = 256 * idx / threads_num;
    int to = 256 * (idx + 1) / threads_num;

//    printf("from [%d] to [%d]\n", from, to);

    int pixel;
    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            pixel = image[i][j];
            if (pixel >= from && pixel < to){
                negative_image[i][j] = 255 - pixel;
            }
        }
    }

    gettimeofday(&stop, NULL);
    long unsigned int* t = malloc(sizeof(long unsigned int));
    *t = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    pthread_exit(t);
}


void* block_method(void* arg) {
    struct timeval stop, start;
    gettimeofday(&start, NULL);

    int idx = *((int *) arg);

    int from = w * idx / threads_num;
    int to = w * (idx + 1) / threads_num;

//    printf("from: %d to: %d \n", x_from, x_to);

    int pixel;
    for (int i = 0; i < h; i++) {
        for (int j = from; j < to; j++) {
            pixel = image[i][j];
            negative_image[i][j] = 255 - pixel;
        }
    }

    gettimeofday(&stop, NULL);
    long unsigned int *t = malloc(sizeof(long unsigned int));
    *t = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    pthread_exit(t);
}


void save(char* filename) {
    FILE *f = fopen(filename, "w");
    if (f == NULL){
        printf("Error while opening output file\n");
        exit(1);
    }

    fprintf(f, "P2\n");
    fprintf(f, "%d %d\n", w, h);
    fprintf(f, "255\n");

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            fprintf(f, "%d ", negative_image[i][j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

int main(int argc, char* argv[]){

    if (argc != 5){
        printf("Wrong number of arguments!\n");
        exit(1);
    }

    threads_num = atoi(argv[1]);
    char* method = argv[2];
    char* input_file = argv[3];
    char* output_file = argv[4];

    if ((strcmp(method, "numbers") != 0) && (strcmp(method, "block") != 0)) {
        printf("Unknown method!\n");
        exit(1);
    }

    load_image(input_file);

    negative_image = calloc(h, sizeof(int *));
    for (int i = 0; i < h; i++) {
        negative_image[i] = calloc(w, sizeof(int));
    }

    pthread_t* threads = calloc(threads_num, sizeof(pthread_t));
    int* threads_idx = calloc(threads_num, sizeof(int));

    struct timeval stop, start;
    gettimeofday(&start, NULL);

    for(int i = 0; i < threads_num; i++){
        threads_idx[i] = i;

        // idx as function argument

        if (strcmp(method, "numbers") == 0){
            pthread_create(&threads[i], NULL, &numbers_method, &threads_idx[i]);
        }

        else if (strcmp(method, "block") == 0){
            pthread_create(&threads[i], NULL, &block_method, &threads_idx[i]);
        }
    }


    // wait for threads to finish and get value passed to pthread_exit() by the thread (return value)
    for(int i = 0; i < threads_num; i++) {
        long unsigned int* t;
        pthread_join(threads[i], (void **) &t);
    }


    gettimeofday(&stop, NULL);
    long unsigned int* t = malloc(sizeof(long unsigned int));
    *t = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    printf("TOTAL TIME: %5lu [μs]\n", *t);

    save(output_file);

    for (int i = 0; i < h; i++) {
        free(image[i]);
        free(negative_image[i]);
    }
    free(image);
    free(negative_image);

    return 0;
}