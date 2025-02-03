#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sysmacros.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int threadCount = 0;
pthread_t* th = NULL;

typedef struct{

    char* filePath;
    char** chArr;
    int chArrSize;

}FileSearch;

typedef struct{
    int count;
    char* searchedString;
}OneStringData;

typedef struct{
    // OneStringData** output;
    int* values;
    char* filePath;
}Result;

void* initFileReading(void* arg){

    FileSearch* fileData = (FileSearch*)arg;
    Result* result = malloc(sizeof(Result));
    if (!result) {
        perror("ERROR ALLOCATING RESULT");
        pthread_exit(NULL);
    }

    printf("file:%s\n",fileData->filePath);

    result->filePath = strdup(fileData->filePath);
    result->values = calloc(fileData->chArrSize, sizeof(int));

    int fd = -1;
    if((fd = open(fileData->filePath,O_RDONLY)) < 0){
        perror("ERROR OPENING FILE");
        free(result->values);
        free(result->filePath);
        free(result);
        pthread_exit(NULL);
    }

    char buffer[1024];
    int readChar = -1;

    while((readChar = read(fd, &buffer, 1024)) > 0){
        for (int i = 0; i < readChar; i++)
        {
            for (int j = 0; j < fileData->chArrSize; j++)
            {
                if(fileData->chArr[j][0] == buffer[i]){
                    // printf("%c",buffer[i]);
                    (result->values[j])++;
                }
            }
        }
    }

    free(fileData->filePath);
    free(fileData);
    close(fd);
    pthread_exit(result);
}

void readDirRecursiv(char* path, char** chArr, int charCount){
    struct dirent* dr = NULL;   
    struct stat sb;  

    DIR* dir = opendir(path);
    if(dir == NULL){
        perror("ERROR OPENING DIR");
        exit(EXIT_FAILURE);
    } 

    while((dr = readdir(dir)) != NULL){

        if(strcmp(dr->d_name,".") == 0 || strcmp(dr->d_name,"..") == 0){
            continue;
        }

        char buffer[512];

        sprintf(buffer,"%s/%s",path,dr->d_name);

        if (lstat(buffer, &sb) == -1) {
            perror("lstat");
            exit(EXIT_FAILURE);
        }

        switch (sb.st_mode & S_IFMT) {
            // case S_IFBLK:  printf("block device\n");            break;
            // case S_IFCHR:  printf("character device\n");        break;
            case S_IFDIR:
                readDirRecursiv(buffer, chArr, charCount);  
                break;
            case S_IFIFO:  printf("FIFO/pipe\n");               break;
            case S_IFLNK:  printf("symlink\n");                 break;
            case S_IFREG:  


                threadCount++;

                th = realloc(th,sizeof(pthread_t) * threadCount);
                if(th == NULL){
                    fprintf(stderr,"ERROR REALOC");
                    exit(EXIT_FAILURE);
                }

                // printf("buffer:%s\n",buffer);

                char* thread_arg = strdup(buffer);
                if (thread_arg == NULL) {
                    perror("ERROR DUPLICATING STRING");
                    free(thread_arg);
                    exit(EXIT_FAILURE);
                }

                FileSearch* fileData = malloc(sizeof(FileSearch));
                if (fileData == NULL) {
                    perror("ERROR ALLOCATING FILESEARCH");
                    exit(EXIT_FAILURE);
                }
                fileData->filePath = malloc(sizeof(char) * 512);
                if (fileData->filePath == NULL) {
                    perror("ERROR ALLOCATING FILEPATH");
                    free(fileData);
                    exit(EXIT_FAILURE);
                }
                snprintf(fileData->filePath,512,"%s",buffer);
                fileData->chArr = chArr;
                fileData->chArrSize = charCount;

                if (pthread_create(&th[threadCount - 1], NULL, initFileReading,fileData) != 0) {
                    exit(-1);
                }
                // printf("regular file\n");            
                break;
            // case S_IFSOCK: printf("socket\n");                  break;
            default:       printf("unknown?\n");                break;
        }
    }

    if(closedir(dir) == -1){
        perror("error closing dir");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv){
    
    if(argc < 5){
        fprintf(stderr,"INVALID ARGS");
        exit(EXIT_FAILURE);
    }

    char* dirName = argv[1];
    char* outputFileName = argv[2];
    int threshold = atoi(argv[3]);
    int charCount = argc - 4;

    // printf("DirName:%s\nOutputFile:%s\nThreshold:%d\ncharCount:%d\n",dirName,outputFileName,threshold,charCount);

    char** chArr = (char**)malloc(sizeof(char*) * charCount);
    for (int i = 0; i < charCount; i++)
    {
        chArr[i] = (char*)malloc(sizeof(char));
    }
    

    int tempIndex = 0;
    for (int i = 4; i < argc; i++)
    {
        chArr[tempIndex] = strdup(argv[i]);
        tempIndex++; 
    }
    
    // for (int i = 0; i < charCount; i++)
    // {
    //     printf("C:%s\n",chArr[i]);
    // }
    

    readDirRecursiv(dirName,chArr,charCount);

    int outFd = -1;

    if((outFd = open(outputFileName, O_WRONLY | O_CREAT | O_TRUNC,S_IRWXU)) < 0){
        perror("ERROR OPENING OUTPUT FILE");
        for (int i = 0; i < threadCount; i++)
        {
            if (pthread_join(th[i], NULL) != 0)
            {
                exit(-1);
            }
        }

        free(th);
        for (int i = 0; i < charCount; i++)
        {
            free(chArr[i]);
        }
        free(chArr);
        
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < threadCount; i++)
    {
        Result* results = malloc(sizeof(Result) * charCount);
        if (pthread_join(th[i], (void**)&results) != 0)
        {
            exit(-1);
        }

        int totalCount = 0;

        printf("Results for file: %s\n", results->filePath);
        for (int j = 0; j < charCount; j++) {
            totalCount += results->values[j];
            printf("Character '%s': %d occurrences\n", chArr[j], results->values[j]);
        }

        printf("TOTAL:%d\n",totalCount);

        if (totalCount > threshold) {
            char symlinkName[512];
            snprintf(symlinkName, sizeof(symlinkName), "%s_th", results->filePath);

            if (symlink(results->filePath, symlinkName) != 0) {
                perror("ERROR CREATING SYMLINK");
            } else {
                printf("Symlink created: %s -> %s\n", symlinkName, results->filePath);
            }
        }

        char output[1024];

        int offset = 0;  // To keep track of the current position in the output buffer

        // Format the file path and start building the output string
        offset += snprintf(output + offset, sizeof(output) - offset, "%s;", results->filePath);

        // Append the values of characters to the output string
        for (int i = 0; i < charCount; i++) {
            offset += snprintf(output + offset, sizeof(output) - offset, "%d;", results->values[i]);
        }

        // Append the total count to the output string
        offset += snprintf(output + offset, sizeof(output) - offset, "%d\n", totalCount);

        write(outFd, output, offset);

        // Eliberăm memoria pentru fiecare rezultat
        free(results->filePath);
        free(results->values);
        free(results);
    }
    
    free(th);
    for (int i = 0; i < charCount; i++)
    {
        free(chArr[i]);
    }
    free(chArr);
    
    return 0;
}
