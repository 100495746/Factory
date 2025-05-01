#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

struct element {
    int num_edition;
    int id_belt;
    int last;
};

struct queue {
    struct element* elements;
    int size;
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
};

// Function declarations (updated signatures!)
int queue_init(struct queue* q, int size_param);
int queue_destroy(struct queue* q);
int queue_put(struct queue* q, struct element* elem);
struct element* queue_get(struct queue* q);
int queue_empty(struct queue* q);
int queue_full(struct queue* q);

#endif

