#include <pthread.h> 
#include <stdio.h>
#include <math.h>

/* this data is shared by the thread(s) */
int threads;
unsigned long long iterations;
double * pi;

void * runner(void * param); /* the thread */

int main(int argc, char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: a.out <iterations> <threads>\n");
        /*exit(1);*/
        return -1;
    }
    if (atoi(argv[1]) < 0 || atoi(argv[2]) < 0) {
        fprintf(stderr, "Arguments must be non-negative\n ");
        /*exit(1);*/
        return -1;
    }

    /* create the thread identifiers */
    pthread_t *tid;
    tid = (pthread_t*) malloc(threads*sizeof(pthread_t)); 

    /* create set of attributes for the thread */
    pthread_attr_t attr;

    /* populate variables... */
    iterations = atoi(argv[1]);
    threads = atoi(argv[2]);
    pi =(double*) calloc(threads,sizeof(double));

    /* get the default attributes */
    pthread_attr_init(&attr);

    /* create threads */
    for(int m = 0; m < threads; m++){
        pthread_create(&(tid[m]), &attr,runner, NULL);
    }
    /* now wait for the threads to exit */
    for(int m = 0;m<threads;m++){
        pthread_join(tid[m], NULL);
    }

        /* compute and print results */
    double result =0;
    for(int m=0;m<threads;m++){
        result+= pi[m];
    }

    printf("pi = %.15f\n",result);

}
/**
 * The thread will begin control in this function
 */
void * runner(void * param) {
    int threadid=pthread_self();

    //complete function
    for(int n = threadid-1;n<=iterations;n+=threads){
        pi[threadid-1] += 4*(pow(-1,n)/((2*n)+1));
    }
    pthread_exit(0);
}
