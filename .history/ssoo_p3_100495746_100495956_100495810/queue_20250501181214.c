#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>


// Global private variables for the queue
//all static so that their modifications dont get destroyed after return
//also, a global static limits visibility to this specific file (important for encapsulation)
// not good idea... they where not using the same queue


//To create a queue
int queue_init(struct queue *q, int size_param){
	q->elements = malloc(size_param * sizeof(struct element)); // alocate memory of "size" (quantity of) elements
	if (q->elements == NULL){
		fprintf(stderr,"[ERROR][queue] There ws an error while using queue.\n");
		return -1;
	}
	q->size = size_param;
	q->head=q->tail=q->count =0;
	// initialize thread variables
	pthread_mutex_init(&q->mutex, NULL);
	pthread_cond_init(&q->not_full, NULL);
	pthread_cond_init(&q->not_empty,NULL);

	return 0;
}

// To Enqueue an element
int queue_put(struct queue *q, struct element* elem) {
	pthread_mutex_lock(&q->mutex); // lock for race conditions
	while (queue_full(q)){
		///printf("[DEBUG][queue] Waiting: queue is empty...\n");
		pthread_cond_wait(&q->not_full, &q->mutex);
	}
	q->elements[q->tail] = *elem;
	q->count++;
	q->tail = (q->tail+1)%q->size;
	pthread_cond_signal(&q->not_empty); // no longer empty
	printf("[OK][queue] Introduced element with id %d in belt %d\n", elem->num_edition, elem->id_belt);
	pthread_mutex_unlock(&q->mutex);

	return 0;
}

// To Dequeue an element.
struct element* queue_get(struct queue *q) {
	pthread_mutex_lock(&q->mutex);

	//cant consume if there is nothing
	while (queue_empty(q)){
		printf("[DEBUG][queue] Waiting: queue is empty...\n");
		pthread_cond_wait(&q->not_empty, &q->mutex); //wait until there's something
	}
	struct element *consumed = malloc(sizeof(struct element));
	if (!consumed){
		fprintf(stderr, "[ERROR][queue] Failed to allocate memory for consumed element\n");
        pthread_mutex_unlock(&q->mutex);
        return NULL;
    }
	*consumed = q->elements[q->head]; //save the address! as this will change with count etc
	q->head = (q->head+1)%q->size;
	q->count--;
	pthread_cond_signal(&q->not_full);
	pthread_mutex_unlock(&q->mutex);

	printf("[OK][queue] Obtained element with id %d in belt %d\n", consumed->num_edition, consumed->id_belt);
	return consumed;
}


//To check queue state
int queue_empty(struct queue *q){
	// no need to lock them, they are already called within a mutex locked function
	return q->count==0;
}

int queue_full(struct queue *q){
	return q->count==q->size;
}

//To destroy the queue and free the resources
int queue_destroy(struct queue *q){
	free(q->elements);
	pthread_mutex_destroy(&q->mutex);
	pthread_cond_destroy(&q->not_full);
	pthread_cond_destroy(&q->not_empty);

	return 0;
}
