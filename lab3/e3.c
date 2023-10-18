// Process Synchronization

// Implement the producer-consumer problem using a bounded-buffer and pthreads that manages race conditions using pthreads mutex locks.

// Your program will run n producers and m consumers at the same time.

#include <pthread.h>

#include <semaphore.h>

#include <stdio.h>

#include <stdlib.h>

#define BUFFER_SIZE 20

int in=0;

int out=0;

int count = 0;

int buffer [BUFFER_SIZE];

pthread_t tid;

pthread_mutex_t mutex;



void insert(int item){

   while (count == BUFFER_SIZE);

   in = (in + 1)%BUFFER_SIZE;

   buffer[in] = item;

   count++;

   printf("in: %d ",in);

   sleep(1); 

}



int remove_item(){

   int item;

   while (count == 0);

   out = (out + 1)%BUFFER_SIZE;

   item = buffer[out];

   count--;

   printf("out: %d ",out);

   sleep(1); 

   return item;

}



void * producer(void * param){

   int item;

   while(1){

      item = rand() % BUFFER_SIZE ;

      while(count>=BUFFER_SIZE);

      pthread_mutex_lock(&mutex);

      insert(item);

      pthread_mutex_unlock(&mutex);

      printf("inserted: %d\n", item);

   }

}



void * consumer(void * param){

   int item;

   while(1){

    while (count == 0);

    pthread_mutex_lock(&mutex);

   	item = remove_item();

   	pthread_mutex_unlock(&mutex);

   	printf("removed: %d\n", item);

   }

   

}



int main(int argc, char * argv[]){

    int producers = atoi(argv[1]);

    int consumers = atoi(argv[2]);

    int i;



    if(pthread_mutex_init(&mutex,NULL)!=0);



    for (i = 0; i < producers; i++)

       pthread_create(&tid, NULL, producer,NULL);

    for (i = 0; i < consumers; i++)

       pthread_create(&tid, NULL, consumer, NULL); 



    pthread_join(tid,NULL);

    pthread_mutex_destroy(&mutex);

}