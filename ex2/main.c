// Sa se scrie un program care primeste ca si argument un caracter si citeste cate o linie de la intrarea standard. Pentru fiecare linie citita programul va crea un thread care va numara de cate ori caracterul dat ca si argument se regaseste in linia citita si va printa acest numar la iesirea standard.

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char charToSearch;

int sharedValue = 0;
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;


void* lineSearch(void* arg){
    char* line = (char*)arg;
    // printf("LINE:%s",line);
    
    int* count = malloc(sizeof(int)); 
    if (!count) {
        perror("Failed to allocate memory for count");
        pthread_exit(NULL);
    }

    *count = 0;

    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] == charToSearch) {
            int r = 0;
            if ((r = pthread_mutex_lock(&my_mutex)) != 0)
            {
                fprintf(stderr, "Mutex lock error: %s\n", strerror(r));
                pthread_exit(NULL);
            }
            (*count)++;
            sharedValue++;
            if ((r = pthread_mutex_unlock(&my_mutex)) != 0)
            {
                fprintf(stderr, "Mutex unlock error: %s\n", strerror(r));
                pthread_exit(NULL);
            }
        }
    }

    // printf("COUNT:%d\n",count);
    pthread_exit(count);
    // return NULL;
}

int main(int argc,char** argv) {

    if(argc != 2){
        fprintf(stderr,"INVALID ARGS");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&my_mutex, NULL);

    pthread_t* th = NULL;
    int threadCount = 0;

    charToSearch = argv[1][0];
    // printf("%c\n",charToSearch);

    char line[1024];
    for(int i = 0;i < 2;i++){
        fgets(line, sizeof(line), stdin);
        threadCount++;
        th = realloc(th,sizeof(pthread_t) * threadCount);
        if(th == NULL){
            fprintf(stderr,"ERROR REALOC");
            exit(EXIT_FAILURE);
        }

        if (pthread_create(&th[threadCount - 1], NULL, lineSearch, (void*)line) != 0)
        {
            exit(-1);
        }
    }

    for (int i = 0; i < threadCount; i++)
    {
        int* result;
        if (pthread_join(th[i], (void**)&result) != 0)
        {
            exit(-1);
        }
        printf("RES:%d\n",*result);
    }
    
    printf("SHARED:%d\n",sharedValue);

    free(th);
    pthread_mutex_destroy(&my_mutex);

    return 0;
}
