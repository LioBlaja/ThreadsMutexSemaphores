#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>

// admit we can store maximum 10 tasks
// the consumer number will be got from console

#define MAX_TASK_NUMBER 10
#define FALSE 0
#define TRUE 1

typedef struct{
    int dummy;
}Element;

int bufferIndex = -1;

sem_t sem_buffer_empty;
sem_t sem_buffer_full;

pthread_mutex_t mutex_buffer_access = PTHREAD_MUTEX_INITIALIZER;

int producerRuning = TRUE;
int consumerRuning = TRUE;

Element tasks[MAX_TASK_NUMBER];


void handler(int signum)
{
    consumerRuning = FALSE;
    producerRuning = FALSE;
}

void* producerThread(void *arg){
    while(producerRuning){

        int randomTime = rand() % 2; 
        Element task = {randomTime * 10};

        if(sem_wait(&sem_buffer_empty) < 0){
            perror("SEM WAIT");
            exit(EXIT_FAILURE);
        }

        int r = -1;

        if ((r = pthread_mutex_lock(&mutex_buffer_access)) != 0){
            fprintf(stderr, "Mutex lock error: %s\n", strerror(r));
            pthread_exit(NULL);
        }
            bufferIndex++;
            tasks[bufferIndex] = task; 
            printf("TASK ADDED with index:%d\n",bufferIndex);

        if ((r = pthread_mutex_unlock(&mutex_buffer_access)) != 0){
            fprintf(stderr, "Mutex unlock error: %s\n", strerror(r));
            pthread_exit(NULL);
        }

        if(sem_post(&sem_buffer_full) < 0){
            perror("SEM POST");
            exit(EXIT_FAILURE);
        }

        sleep(randomTime);

    }

    printf("PRODUCER STOPPED\n");

    return NULL;
}

void* consumerThread(void *arg){

    Element task;

    while(consumerRuning){

        if(sem_wait(&sem_buffer_full) < 0){
            perror("SEM WAIT");
            exit(EXIT_FAILURE);
        }
        int randomTime = rand() % 2; 
        int r = -1;
        
        if ((r = pthread_mutex_lock(&mutex_buffer_access)) != 0){
            fprintf(stderr, "Mutex lock error: %s\n", strerror(r));
            pthread_exit(NULL);
        }
            task = tasks[bufferIndex];
            bufferIndex--;
            // bufferIndex++;
            // tasks[bufferIndex] = task; 
            // printf("TASK ADDED with index:%d\n",bufferIndex);
        if ((r = pthread_mutex_unlock(&mutex_buffer_access)) != 0){
            fprintf(stderr, "Mutex unlock error: %s\n", strerror(r));
            pthread_exit(NULL);
        }

        if(sem_post(&sem_buffer_empty) < 0){
            perror("SEM POST");
            exit(EXIT_FAILURE);
        }
        printf("CONSUMED TASK WITH VALUE: %d INDEX: %d\n",task.dummy,bufferIndex + 1);
        sleep(randomTime);
    }

    return NULL;
}

int main(int argc,char**argv){
    
    if(argc != 2){
        fprintf(stderr,"INVALID USAGE");
        exit(EXIT_FAILURE);
    }

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = handler;
    if (sigaction(SIGTERM, &action, NULL) < 0)
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&mutex_buffer_access, NULL);
    pthread_mutex_init(&mutex_buffer_access, NULL);

    if (sem_init(&sem_buffer_full, 0, 0) < 0)
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    if (sem_init(&sem_buffer_empty, 0, MAX_TASK_NUMBER) < 0)
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    pthread_t* producersThHandlers = (pthread_t*)malloc(3*sizeof(pthread_t));
    if(producersThHandlers == NULL){
        perror("MALLOC");
        exit(EXIT_FAILURE);
    }
    int r = -1;

    for (int i = 0; i < 3; i++)
    {
        if ((r = pthread_create(&producersThHandlers[i], NULL, producerThread, NULL)) != 0)
        {
            fprintf(stderr, "%s\n", strerror(r));
            exit(EXIT_FAILURE);
        }
    }

    consumerRuning = TRUE;
    producerRuning = TRUE;

    pthread_t* consumersThHandlers = (pthread_t*)malloc(atoi(argv[1])*sizeof(pthread_t));
    if(consumersThHandlers == NULL){
        perror("MALLOC");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < atoi(argv[1]); i++)
    {
        if ((r = pthread_create(&consumersThHandlers[i], NULL, consumerThread, NULL)) != 0)
        {
            fprintf(stderr, "%s\n", strerror(r));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < atoi(argv[1]); i++){
        pthread_join(consumersThHandlers[i], NULL);
    }

    sem_destroy(&sem_buffer_full);
    sem_destroy(&sem_buffer_empty);

    printf("Program ended\n");

    free(consumersThHandlers);

    return 0;
}
