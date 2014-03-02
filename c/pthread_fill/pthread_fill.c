#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>

#define SEQUENTIAL_VAL 1
#define PTHREAD_VAL 2

void *pthread_fill (void *arg);
int check_array (uint8_t array[], uint32_t size, uint32_t n);
void press_enter ();
double wall_time ();

typedef struct {
    uint32_t id;
    uint8_t *fill_this;
    uint32_t start;
    uint32_t end;
} pthread_args;

int main (int arg_count, char *arg_values[]) {
    /* Grab number of threads from invocation args */
    if (arg_count < 2) {
        printf("Usage: pthread_fill <num_threads>\n");
        exit(1);
    }

    uint32_t num_threads = atoi(arg_values[1]);

    if (num_threads < 1 || num_threads > (1 << 16)) {
        printf("Invalid number of threads, exiting.\n");
        exit(2);
    }

    /* Allocate space for a 500MB array */
    uint32_t size = (1 << 20) * 500;
    uint8_t *fill_this = malloc(size * sizeof(uint8_t));

    /* Initialize the array to SEQUENTIAL_VAL using the single main thread */
    printf("Filling a 500MB array in the master thread ...\n");
    press_enter();
    uint32_t i;
    double start_time = wall_time();
    for (i = 0; i < size; i++) {
        fill_this[i] = SEQUENTIAL_VAL;
    }
    double sequential_runtime = wall_time() - start_time;
    printf("Done in %f seconds\n", sequential_runtime);

    /* Check if the initilization was successful */
    if (check_array(fill_this, size, SEQUENTIAL_VAL)) {
        printf("Array check failed, exiting.\n");
        exit(3);
    }
    printf("Array check passed\n\n");

    /* Initialize the array to PTHREAD_VAL using pthreads */
    pthread_t threads[num_threads];

    printf("Filling a 500MB array using %d threads ...\n", num_threads);
    press_enter();
    start_time = wall_time();
    for (i = 0; i < num_threads; i++) {
        pthread_args *arg = malloc(sizeof(pthread_args));
        arg->id = i;
        arg->fill_this = fill_this;
        uint32_t chunk_size = size / num_threads;
        arg->start = chunk_size * i;
        arg->end = chunk_size * (i + 1) - 1;
        pthread_create(&threads[i], NULL, pthread_fill, arg);
    }

    /* Joining on the threads before completing timing */
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    double threaded_runtime = wall_time() - start_time;
    printf("Done in %f seconds\n", threaded_runtime);

    /* Check if the initilization was successful */
    if (check_array(fill_this, size, PTHREAD_VAL)) {
        printf("Array check failed, exiting.\n");
        exit(3);
    }
    printf("Array check passed\n\n");

    /* Calculate and print speedup */
    printf("Speedup %f\n", sequential_runtime / threaded_runtime);

    return 0;
}

void *pthread_fill (void *arg) {
    pthread_args *args = (pthread_args *)arg;
    uint32_t i;
    for (i = args->start; i <= args->end; i++) {
        args->fill_this[i] = PTHREAD_VAL;
    }
    return NULL;
}

int check_array (uint8_t array[], uint32_t size, uint32_t n) {
    uint32_t i;
    for (i = 0; i < size; i++) {
        if (array[i] != n) {
            return -1;
        }
    }

    return 0;
}

void press_enter () {
    printf("Press ENTER to continue");

    /* The proper way to clear a console buffer.
       fflush operates on output file descriptors only */
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

double wall_time () {
    struct timeval time;
    if (gettimeofday(&time, NULL)){
        return -1;
    }
    return (double)time.tv_sec + (double)time.tv_usec * 0.000001;
}

