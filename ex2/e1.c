#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_t *tid;
pthread_mutex_t *toys;

struct kId {
    int id;
    int M;
};

void* kid(void* arg){
    // START ADDING YOUR CODE FROM HERE
    struct kId* kidInfo = (struct kId*)arg;
    int kidId = kidInfo->id;
    int toyId = kidId / 2;

    // Wait for the previous kid to finish playing with the assigned toy
    pthread_mutex_lock(&toys[toyId]);

    printf("Kid %d is playing with toy %d\n", kidId, toyId);

    // Simulate playtime
    sleep(1);

    printf("Kid %d finished playing\n", kidId);

    // Release the toy for the next kid
    pthread_mutex_unlock(&toys[toyId]);

    pthread_exit(0);
    // TO HERE
}

int main(int argc, char *argv[]){
    int i;
    int error;
    int N, M;
    struct kId *tempId;

    if(argc > 2){
        printf("Must supply 1 parameter! (Number of kids)\n");
        return 0;
    }
    N = atoi(argv[1]);
    tid = (pthread_t*)malloc(N*sizeof(pthread_t));
    tempId = (struct kId*)malloc(N*sizeof(struct kId));
    M = N/2;
    if(N%2) M++;
    toys = (pthread_mutex_t*)calloc(M,sizeof(pthread_mutex_t));
    
    for(i = 0; i < M; i++){
        if (pthread_mutex_init(&toys[i], NULL) != 0) {
            printf("\n mutex init has failed\n");
            return 1;
        }
    }
    i = 0;
    while (i < N) {
        tempId[i].id = i;
        tempId[i].M = M;
        error = pthread_create(&(tid[i]), NULL, &kid, &tempId[i]);
        if (error != 0)
            printf("\nThread can't be created : [%d]", error);
        i++;
    }
    i = 0;
    while (i < N) {
        pthread_join(tid[i++], NULL);
    }
  
    free(tid);
    tid = NULL;
    for(i = 0; i < M; i++){
        pthread_mutex_destroy(&toys[i]);
    }
    free(toys);
    toys = NULL;
    free(tempId);
    tempId = NULL;
    return 0;
}