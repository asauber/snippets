#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *pthread_hello (void *arg);

int main (int arg_count, char *arg_values[]) {
    if (arg_count < 2) {
        printf("Usage: pthreads <num_threads>\n");
        exit(1);
    }

    int num_threads = atoi(arg_values[1]);

    printf("%d threads will be launched\n", num_threads);

    if (num_threads < 1 || num_threads > (1 << 16)) {
        printf("Invalid number of threads, exiting.\n");
        exit(2);
    }

    pthread_t threads[num_threads];

    int i;
    for (i = 0; i < num_threads; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, pthread_hello, id);
    }

    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void *pthread_hello (void *arg) {
    int *id = (int *)arg;
    printf("Hello from thread %d\n", *id);
    return NULL;
}

