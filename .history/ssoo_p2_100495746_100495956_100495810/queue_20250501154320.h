#ifndef HEADER_FILE
#define HEADER_FILE

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
int queue_init (int size);
int queue_destroy (void);
int queue_put (struct element* elem);
struct element * queue_get(void);
int queue_empty (void);
int queue_full(void);

#endif
