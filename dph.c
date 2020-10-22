/**
 * Project 2 -- The Dining-Philosophers Problem
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define N_THREADS 5

enum {THINKING, HUNGRY, EATING} state[5];

pthread_cond_t self[N_THREADS];  // conditional variables for behalf change
pthread_mutex_t mutex[N_THREADS];  // mutex for conditional variables
int idx[N_THREADS];  // philosopher index

void *philosopher(void *param);
void pickup_forks(int idx);  // a philosopher is hungry and wants to eat
void return_forks(int idx);  // a philosopher have ate and wants to think
void test(int idx);  // test if a philosopher can eat or not

int main(int argc, char **argv) {
    pthread_t tids[N_THREADS];
    int i;
    for (i = 0; i < N_THREADS; i++) {
        idx[i] = i;
        int t = pthread_create(&tids[i], NULL, philosopher, &idx[i]);
        if (t) {
            fprintf(stderr, "Error - pthread_create() return code: %d\n", t);
            return -1;
        }
    }
    for (i = 0; i < N_THREADS; i++) {
        pthread_join(tids[i], NULL);
    }

    return 0;
}

void *philosopher(void *param) {
    int idx = *((int *)param);
    while (1) {
        /* Try to pickup a chopstick*/
        pickup_forks(idx);
        printf("The philosopher %d is eating...\n", idx);
        /* Eat a while */
        srand((unsigned)time(NULL));
        int sec = rand() % 4 + 1;
        sleep(sec);
        /* Return a chopstick */
        return_forks(idx);
        printf("The philosopher %d is thinking...\n", idx);
        /* Think a while */
        srand((unsigned)time(NULL));
        sec = rand() % 4 + 1;
        sleep(sec);
    }
    pthread_exit(0);
}

void pickup_forks(int idx){
    state[idx] = HUNGRY;
    test(idx);
    pthread_mutex_lock(&mutex[idx]);
    while (state[idx] != EATING) {
        pthread_cond_wait(&self[idx], &mutex[idx]);
    }
    pthread_mutex_unlock(&mutex[idx]);
}

void return_forks(int idx) {
    state[idx] = THINKING;
    test((idx + 4) % 5);
    test((idx + 1) % 5);
}

void test(int idx){
    // A philosopher can eat when he wants to eat and his neighbors aren't eating.
    if ((state[(idx + 4) % 5] != EATING) && (state[idx] == HUNGRY) && (state[(idx + 1) % 5] != EATING)) {
        pthread_mutex_lock(&mutex[idx]);
        state[idx] = EATING;
        pthread_cond_signal(&self[idx]);
        pthread_mutex_unlock(&mutex[idx]);
    }
}
