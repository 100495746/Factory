#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"

// Global private variables for the queue
static struct element* elements = NULL;
static int size = 0;
static int head = 0;
static int tail = 0;
static int count = 0;
static pthread_mutex_t mutex;
static pthread_cond_t not_full;
static pthread_cond_t not_empty;



//To create a queue
int queue_init(int size){
	elements = malloc(size * sizeof(struct element)); // alocate memory of "size" (quantity of) elements
	if (elements == NULL){
		printf("Error alocating memory\n");
		return -1;
	}
	head=tail=count =0;
	// initialize thread variables
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&not_full, NULL);
	pthread_cond_init(&not_empty,NULL);

	return 0;
}


// To Enqueue an element
int queue_put(struct element* x) {
	pthread_mutex_lock(&mutex); // lock for race conditions
	while (count == size){
		pthread_cond_wait(&not_full, &mutex);
	}
	elements[tail] = *x;
	count++;
	tail = (tail+1)%size;
	pthread_cond_signal(&not_empty); // no longer empty
	pthread_mutex_unlock(&mutex);

	return 0;
}


// To Dequeue an element.
struct element* queue_get(void) {
	pthread_mutex_lock(&mutex);
	
	while (count==0){
		pthread_cond_wait(&not_empty, &mutex);
	}
	struct element* consumed = elements[head]; //?? how do i "eliminate" it
	count--;
	head = (head+1)%size;
	pthread_cond_signal(&not_full);
	pthread_mutex_unlock(&mutex);

	return NULL;
}


//To check queue state
int queue_empty(void){
	return 0;
}

int queue_full(void){
	return 0;
}

//To destroy the queue and free the resources
int queue_destroy(void){
	free(elements);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&not_full);
	pthread_cond_destroy(&not_empty);

	return 0;
}
