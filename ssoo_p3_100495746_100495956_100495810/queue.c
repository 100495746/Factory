#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>

int queue_init(struct queue *q, int size_param) {
    q->elements = malloc(size_param * sizeof(struct element));
    if (q->elements == NULL) {
        fprintf(stderr, "[ERROR][queue] There was an error while using queue.\n");
        return -1;
    }
    q->size = size_param;
    q->head = q->tail = q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    return 0;
}

int queue_put(struct queue *q, struct element* elem) {
    pthread_mutex_lock(&q->mutex);
    while (queue_full(q)) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    q->elements[q->tail] = *elem;
    q->count++;
    q->tail = (q->tail + 1) % q->size;
    pthread_cond_signal(&q->not_empty);
    printf("[OK][queue] Introduced element with id %d in belt %d.\n", elem->num_edition, elem->id_belt);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

struct element* queue_get(struct queue *q) {
    pthread_mutex_lock(&q->mutex);
    while (queue_empty(q)) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    struct element *consumed = malloc(sizeof(struct element));
    if (!consumed) {
        fprintf(stderr, "[ERROR][queue] There was an error while using queue with id: %d.\n", q->elements[q->head].id_belt);
        pthread_mutex_unlock(&q->mutex);
        return NULL;
    }
    *consumed = q->elements[q->head];
    q->head = (q->head + 1) % q->size;
    q->count--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);

    printf("[OK][queue] Obtained element with id %d in belt %d.\n", consumed->num_edition, consumed->id_belt);
    return consumed;
}

int queue_empty(struct queue *q) {
    return q->count == 0;
}

int queue_full(struct queue *q) {
    return q->count == q->size;
}

int queue_destroy(struct queue *q) {
    free(q->elements);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_full);
    pthread_cond_destroy(&q->not_empty);
    return 0;
}