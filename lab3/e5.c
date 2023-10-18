// Modify your producer-consumer implementation so that it uses monitors to handle race conditions instead of semaphores or mutexes. 
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

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;

int full, empty;



void insert(int item){

   while (count == BUFFER_SIZE);

   wait(&full);

   in = (in + 1)%BUFFER_SIZE;

   buffer[in] = item;

   count++;

   if(count==1)

    signal(&empty);

   sleep(1); 

}



int remove_item(){

   int item;



   while (count == 0);

   wait(&empty);

   item = buffer[out];

   count--;

   if(count==BUFFER_SIZE-1)

    signal(&full);

   sleep(1); 

   return item;



}



void * producer(void * param){

   int item;



   while(1){

      item = rand() % BUFFER_SIZE ;

      insert(item);

      printf("inserted: %d\n", item);

   }



}



void * consumer(void * param){

   int item;



   while(1){

   	item = remove_item();

   	printf("removed: %d\n", item);

   }



}







int main(int argc, char * argv[]){

    int producers = 2;//atoi(argv[1]);

    int consumers = 1;//atoi(argv[2]);

    int i;



    if(pthread_mutex_init(&mutex,NULL)!=0);

    for (i = 0; i < producers; i++)

       pthread_create(&tid, NULL, producer,NULL);

    for (i = 0; i < consumers; i++)

       pthread_create(&tid, NULL, consumer, NULL); 



    pthread_join(tid,NULL);

    pthread_mutex_destroy(&mutex);



}