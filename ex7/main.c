// 7. Sa se scrie un program care primeste ca si argument un fisier text si un numar N reprezentat un numar de thread-uri. Programul va imparti fisierul in N partitii egale si pentru fiecare partitie va lansa un thread care va realiza histograma caracterelor din fisier. Thread-urile vor completa rezultatul intr-un tablou comun de histrograma. 

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

int* histograma = NULL;
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
    int chunkSize;
    char* buffer;
}HistogramInputData;

int output[128] = {0};

void* computeHistogram(void* arg){

    HistogramInputData* data = (HistogramInputData*)arg;
    
    // printf("INPUT:%s\n",data->buffer);
    // printf("SIZE:%d\n",data->chunkSize);

    for (size_t i = 0; i < data->chunkSize; i++)
    {
        int dec = (int)data->buffer[i];
        // printf("%d ",dec);
        int r = 0;
        if ((r = pthread_mutex_lock(&my_mutex)) != 0) {
            fprintf(stderr, "Mutex lock error: %s\n", strerror(r));
            pthread_exit(NULL);
        }
        output[dec]++;
        if ((r = pthread_mutex_unlock(&my_mutex)) != 0) {
            fprintf(stderr, "Mutex unlock error: %s\n", strerror(r));
            pthread_exit(NULL);
        }
    }
    
    free(data);
    return NULL;
}

int main(int argc, char** argv){
    
    if(argc != 3){
        fprintf(stderr,"INVALID ARGUMENTS");
        exit(EXIT_FAILURE);
    }

    char* inputFilePath = argv[1];
    // printf("FILE:%s\n",inputFilePath);
    int N = atoi(argv[2]);
    pthread_mutex_init(&my_mutex, NULL);

    struct stat sb;

    if(stat(inputFilePath, &sb) < 0){
        perror("ERROR GETING STATS ABOUT INPUT FILE");
        exit(EXIT_FAILURE);
    }

    // printf("Size:%ld\n",sb.st_size);

    int chunkSize = sb.st_size / N;
    int lastChunkSize = -1;
    if(sb.st_size % N != 0){
       lastChunkSize = (sb.st_size - chunkSize * N) + chunkSize;
    //    printf("LAST CHUNK SIZE:%d\n",lastChunkSize);
    }

    pthread_t* th = (pthread_t*)malloc(sizeof(pthread_t) * N);

    if(th == NULL){
        fprintf(stderr,"ERROR MEMMORY ALLOC FOR THREADS HANDLERS");
        exit(EXIT_FAILURE);
    }

    int fdIn = -1;

    if((fdIn = open(inputFilePath,O_RDONLY)) < 0){
        perror("ERROR OPENING FILE");
        free(th);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N; i++) {

        char* buffer = NULL;
        int variableChunkSize = chunkSize;

        if(i == (N - 1) && lastChunkSize != -1){
            variableChunkSize = lastChunkSize;    
        }

        buffer = (char*)malloc(sizeof(char) * variableChunkSize);

        if(buffer == NULL) {
            fprintf(stderr,"ERROR MEMMORY ALLOC FOR BUFFER");
            free(th);
            close(fdIn);
            exit(EXIT_FAILURE);
        }

        if(read(fdIn,buffer,variableChunkSize) < 0){
            perror("ERROR READING INPUT FILE");
            free(th);
            close(fdIn);
            exit(EXIT_FAILURE);
        }

        HistogramInputData* data = (HistogramInputData*)malloc(sizeof(HistogramInputData));
        data->chunkSize = variableChunkSize;
        data->buffer = strdup(buffer); 

        if (pthread_create(&th[i], NULL, computeHistogram, data) != 0) {
            exit(-1);
        }
    }

    for (int i = 0; i < N; i++)
    {
        if (pthread_join(th[i], NULL) != 0) {
            exit(-1);
        }
    }
    
    for (int i = 0; i < 128; i++)
    {
        if (output[i] > 0) { // Print only characters with non-zero frequencies
            printf("Character '%c' (ASCII %d): %d occurrences\n", (char)i, i, output[i]);
        }
    }
    
    free(th);
    pthread_mutex_destroy(&my_mutex);

    return 0;
}
