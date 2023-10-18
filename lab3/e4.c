// Modify your producer-consumer implementation so that it uses semaphores to handle race conditions instead of mutexes.
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

sem_t mutex;



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

      sem_wait(&mutex);

      insert(item);

      sem_post(&mutex);

      printf("inserted: %d\n", item);

      



   }



}



void * consumer(void * param){

   int item;



   while(1){

    while (count == 0);

    sem_wait(&mutex);

   	item = remove_item();

   	sem_post(&mutex);

   	printf("removed: %d\n", item);



   }

}



int main(int argc, char * argv[]){

    int producers = 2;

    int consumers = 1;

    int i;



    if(sem_init(&mutex, 0, BUFFER_SIZE)!=0);

    for (i = 0; i < producers; i++)

       pthread_create(&tid, NULL, producer,NULL);

    for (i = 0; i < consumers; i++)

       pthread_create(&tid, NULL, consumer, NULL); 



    pthread_join(tid,NULL);



    sem_destroy(&mutex);



}