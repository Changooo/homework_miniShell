#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int inPoint = 0;
int pointPerThread = 0;
int threadNum = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *count_point() {
    int in = 0;
    int s = rand();
    for (int i = 0; i < pointPerThread; i++) {
        double x = (double)rand_r(&s) / (double)RAND_MAX;
        double y = (double)rand_r(&s) / (double)RAND_MAX;
        if (x*x + y*y < 1) in++;
    }
    
    pthread_mutex_lock(&mutex);
    inPoint += in;
    pthread_mutex_unlock(&mutex);
}

int main(int argc, const char *argv[])
{
    threadNum = atol(argv[1]);
    pointPerThread = atoi(argv[2]);

    pthread_t *threads = malloc(threadNum * sizeof(pthread_t));

    for (int i = 0; i < threadNum; i++) {
        pthread_create(&threads[i], NULL, count_point, (void *) NULL);
    }

    for (int i = 0; i < threadNum; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    free(threads);

    int totalPoint = pointPerThread * threadNum;
    double pi = 4.0 * (double)inPoint / (double)totalPoint;
    printf("%f\n", pi);

    return 0;
}
