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
	str


	return 0;
}


// To Enqueue an element
int queue_put(struct element* x) {
	return 0;
}


// To Dequeue an element.
struct element* queue_get(void) {
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
	return 0;
}
