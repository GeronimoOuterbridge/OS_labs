#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

#define PHILOCOUNT 10
#define EATING 0
#define THINKING 1

// Philosophers may take only the chopsticks on their corresponding index, and the previous, in a circular fashion.
sem_t chopsticks[PHILOCOUNT];
int philosopher_states[PHILOCOUNT];
// To prevent starvation, we'll have an array that tells us how many times the philosophers to the sides of a philosopher have eaten since that philosopher last ate.
// That way, we can make a philosopher wait before grabbing chopsticks

sem_t *get_left(int i)
{
    int sem_index = (i - 1) % PHILOCOUNT;
    return &chopsticks[sem_index];
}

sem_t *get_right(int i)
{
    int sem_index = i;
    return &chopsticks[sem_index];
}

void *philosopher(void *args)
{
    int my_id = *((int *)args);
    sem_t* left_chopstick;
    sem_t* right_chopstick;

    left_chopstick = get_left(my_id);
    right_chopstick = get_right(my_id);

    while (1)
    {
        int *state = &philosopher_states[my_id];

        if (*state == EATING)
        {
            printf("[%d] Eating!\n", my_id);
            sleep(0.5);
            sem_post(left_chopstick);
            sem_post(right_chopstick);
            *state = THINKING;
        }
        else
        {
            printf("[%d] Thinking!\n", my_id);
            sem_wait(left_chopstick);
            sem_wait(right_chopstick);
            *state = EATING;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int i;
    pthread_attr_t default_attributes;
    pthread_t threads[PHILOCOUNT];
    int philosopher_ids[PHILOCOUNT];

    // Initialize semaphores
    for (i = 0; i < PHILOCOUNT; i++)
    {
        sem_t chopstick = chopsticks[i];
        sem_init(&chopstick, 1, 1);
    }

    // Create argument array
    for (i = 0; i < PHILOCOUNT; i++)
    {
        philosopher_ids[i] = i;
    }

    // Create threads
    pthread_attr_init(&default_attributes);
    for (i = 0; i < PHILOCOUNT; i++)
    {
        pthread_t* thread = &threads[i];
        void* args = (void*)&philosopher_ids[i];
        pthread_create(thread, &default_attributes, philosopher, args);
    }

    // They never really finish but we will join them anyway
    for (i = 0; i < PHILOCOUNT; i++)
    {
        pthread_t thread = threads[i];
        pthread_join(thread, NULL);
    }

    return 0;
}
