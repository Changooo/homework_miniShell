#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define QSIZE 5
#define LOOP 30

typedef struct {
   int data[QSIZE];
   int index;
   int count;
   pthread_mutex_t lock;
   pthread_cond_t notfull;
   pthread_cond_t notempty;
} queue_t;

void *produce (void *args);
void *consume (void *args);
void put_data (queue_t *q, int d);
int get_data (queue_t *q);

queue_t *qinit() 
{
   queue_t *q;

   q = (queue_t *) malloc(sizeof(queue_t));
   q->index = q->count = 0;
   pthread_mutex_init(&q->lock, NULL);
   pthread_cond_init(&q->notfull, NULL);
   pthread_cond_init(&q->notempty, NULL);

   return q;
}

void qdelete(queue_t *q) 
{
   pthread_mutex_destroy(&q->lock);
   pthread_cond_destroy(&q->notfull);
   pthread_cond_destroy(&q->notempty);
   free(q);
}

int main () 
{
   queue_t *q;
   pthread_t producer, consumer;

   q = qinit();

   pthread_create(&producer, NULL, produce, (void *)q);
   pthread_create(&consumer, NULL, consume, (void *)q);

   pthread_join (producer, NULL);
   pthread_join (consumer, NULL);
   
   qdelete(q);
}

void *produce(void *args) 
{
   int i, d;
   queue_t *q = (queue_t *)args;
   for (i = 0; i < LOOP; i++) {
      d = i;
      put_data(q, d);
      printf("put data %d to queue\n", d);
   }
   pthread_exit(NULL);
}

void *consume(void *args) 
{
   int i, d;
   queue_t *q = (queue_t *)args;
   for (i = 0; i < LOOP; i++) {
      d = get_data(q);
      printf("got data %d from queue\n", d);
   }
   pthread_exit(NULL);
}

void put_data(queue_t *q, int d) 
{
	pthread_mutex_lock(&q->lock);
	
	while (1) {
		if(q->count - QSIZE) break;
		pthread_cond_wait(&q->notfull, &q->lock);
	}

	int quo = ( q->index + q->count ) % QSIZE;
	q->data[quo] = d;
	q->count += 1;

	pthread_cond_signal(&q->notempty);
	pthread_mutex_unlock(&q->lock);   
}
int get_data(queue_t *q) 
{
	pthread_mutex_lock(&q->lock);
	
	while (1) {
		if(q->count) break;
		pthread_cond_wait(&q->notempty, &q->lock);
	}

	int res = q->data[q->index];
	q->index = (q->index + 1) % QSIZE;
	q->count -= 1;

	pthread_cond_signal(&q->notfull);
	pthread_mutex_unlock(&q->lock);
	return res;
}
