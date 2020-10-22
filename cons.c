/**
 * Project 1 - consumer
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <math.h>

#define N_THREADS 3 
#define BUFFER_SIZE 20

typedef int item;
typedef struct buffer_struct {
    int in;
    int out;
    item buffer[BUFFER_SIZE];
} buffer_struct;

void *consume(void *param);
double consume_interval_time(double lambda_c);

double lambda_c;
sem_t *full;
sem_t *empty;
sem_t *mutex;  // mutex for buffer_struct
void *ptr;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: ./prod lambda_c\n");
        return -1;
    }
    lambda_c = atof(argv[1]);

    full = sem_open("full", O_CREAT, 0666, 0);
    empty = sem_open("empty", O_CREAT, 0666, 0);;
    mutex = sem_open("mutex", O_CREAT, 0666, 0);  

    /* open the shared memory segment */
	int shm_fd = shm_open("buffer", O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1) {
		printf("shared memory failed\n");
		exit(-1);
	}
	/* now map the shared memory segment in the address space of the process */
	ptr = mmap(0, sizeof(buffer_struct), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}

    pthread_t tids[N_THREADS];
    int i;
    for (i = 0; i < N_THREADS; i++) {
        int t = pthread_create(&tids[i], NULL, consume, NULL);
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

void *consume(void *param) {
    while (1) {
        double interval_time_s = consume_interval_time(lambda_c);
        usleep((unsigned int)(interval_time_s * 1e6));
        buffer_struct *shm_ptr = (buffer_struct *)ptr;
        sem_wait(full);
        sem_wait(mutex);
        item digit = shm_ptr->buffer[shm_ptr->out];
        shm_ptr->out = (shm_ptr->out + 1) % BUFFER_SIZE;
        printf("pid=[%ld], tid=[%ld]: consume digit %3d from the buffer[%d]\n", getpid(), pthread_self(), digit, shm_ptr->out);
        sem_post(mutex);
        sem_post(empty);
    }
    pthread_exit(0);
}

double consume_interval_time(double lambda_c) {
    double z;
    do {
        z = ((double)rand() / RAND_MAX);
    }
    while((z==0) || (z == 1));
    return (-1/lambda_c * log(z));
}
