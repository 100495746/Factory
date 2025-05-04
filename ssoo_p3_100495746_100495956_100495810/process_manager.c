/*
 *
 * process_manager.c
 *
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <stddef.h>
 #include <pthread.h>
 #include "queue.h"
 #include <semaphore.h>
 
 #define NUM_THREADS 2 // One producer and one consumer
 
 struct th_args_pm {
	 int belt_id;
	 int to_produce;
	 int is_producer;
	 struct queue *q;
 };
 
 void *Produce(void *arg) {
	 struct th_args_pm *args = (struct th_args_pm*) arg;
	 for (int i = 0; i < args->to_produce; i++) {
		 struct element elem = {0};
		 elem.num_edition = i;
		 elem.id_belt = args->belt_id;
		 elem.last = (i == args->to_produce - 1) ? 1 : 0;
		 queue_put(args->q, &elem);
	 }
	 free(arg);
	 pthread_exit(NULL);
 }
 
 void *Consume(void *arg) {
	 struct th_args_pm *args = (struct th_args_pm*) arg;
	 for (int i = 0; i < args->to_produce; i++) {
		 struct element* elem = queue_get(args->q);
		 free(elem);
	 }
	 free(arg);
	 pthread_exit(NULL);
 }
 
 int process_manager(int id, int belt_size, int items_to_produce) {
	 pthread_t threads[NUM_THREADS];
	 
	 if (id <= 0 || belt_size <= 0 || items_to_produce <= 0) {
		 fprintf(stderr, "[ERROR][process_manager] Arguments not valid with id %d.\n", id);
		 return -1;
	 }
 
	 struct queue q;
	 if(queue_init(&q, belt_size) != 0) {
		 fprintf(stderr,"[ERROR][process_manager] There was an error executing process manager with id %d.\n", id);
		 return -1;
	 }
	 printf("[OK][process_manager] Belt with id %d has been created with a maximum of %d elements.\n", id, belt_size);
	 printf("[OK][process_manager] Process_manager with id %d waiting to produce %d elements.\n", id, items_to_produce);
 
	 struct th_args_pm *prod_args = malloc(sizeof(struct th_args_pm));
	 if(prod_args == NULL) {
		 fprintf(stderr,"[ERROR][process_manager] There was an error executing process manager with id %d.\n", id);
		 return -1;
	 }
 
	 prod_args->belt_id = id;
	 prod_args->to_produce = items_to_produce;
	 prod_args->is_producer = 1;
	 prod_args->q = &q;
	 
	 struct th_args_pm *cons_args = malloc(sizeof(struct th_args_pm));
	 if(cons_args == NULL) {
		 fprintf(stderr,"[ERROR][process_manager] There was an error executing process manager with id %d.\n", id);
		 free(prod_args);
		 return -1;
	 }
 
	 cons_args->belt_id = id;
	 cons_args->to_produce = items_to_produce;
	 cons_args->is_producer = 0;
	 cons_args->q = &q;
 
	 int rp, rc;
	 rp = pthread_create(&threads[0], NULL, Produce, prod_args);
	 rc = pthread_create(&threads[1], NULL, Consume, cons_args);
	 
	 if (rp || rc) {
		 fprintf(stderr,"[ERROR][process_manager] There was an error executing process manager with id %d.\n", id);
		 free(prod_args);
		 free(cons_args);
		 return -1;
	 }
 
	 pthread_join(threads[0], NULL);
	 pthread_join(threads[1], NULL);
 
	 printf("[OK][process_manager] Process_manager with id %d has produced %d elements.\n", id, items_to_produce);
	 queue_destroy(&q);
	 
	 return 0;
 }