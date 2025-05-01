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
#include "process_manager.h"
#include <semaphore.h>

#define NUM_THREADS 2 //one consumer and one producer per belt


void *Produce(void *arg){
	struct th_args_pm *args = (struct th_args_pm*) arg;
	for (int i = 0; i < args->to_produce; i++) {
		struct element elem = {0}; // to initialize before giving it as argument, it deactivates warnings
		elem.num_edition = i; // a "personal" id for the element
		elem.id_belt = args->belt_id;
		elem.last = (i == args->to_produce - 1) ? 1 : 0; // last if there are no more
		//printf("[DEBUG][Produce] Sending item %d to belt %d\n", i, args->belt_id);
		queue_put(args->q,&elem);
	}
	free(arg); 
	pthread_exit(NULL);

}

void *Consume(void *arg){
	struct th_args_pm *args = (struct th_args_pm*) arg;
	for (int i = 0; i < args->to_produce; i++) {
		//printf("[DEBUG][Consume] Waiting to receive item %d from belt %d\n", i, args->belt_id);		
		struct element* elem = queue_get(args->q);
		free(elem); // we "consume" the elem, which was a struct with allocated memory we need to free
	}
	free(arg);
	pthread_exit(NULL);
}

int process_manager (int id, int belt_size, int items_to_produce ){
	//printf("[DEBUG] Producer will create %d elements on belt %d\n", items_to_produce, id);
	//printf("[DEBUG] Consumer will expect %d elements on belt %d\n", items_to_produce, id);

	pthread_t threads[NUM_THREADS]; //1 prod, 1 cons
   	//long t; // numbers the thread (thread id)
	if (id <= 0 || belt_size <= 0 || items_to_produce <= 0) {
		fprintf(stderr, "[ERROR][process_manager] Arguments not valid with id %d\n", id);
		return -1;
	}

	//using the definded struct/function in queue.c
	
	struct queue q; // create a queue as belt
	if(queue_init(&q, belt_size) !=0){
		fprintf(stderr,"[ERROR][process_manager] There was an error executing process manager with id %d\n", id);
		return -1;
	};
	printf("[OK][process_manager] Belt with id %d has been created with a maximum of %d threads\n", id, items_to_produce);
	
	// these arguments need to be pointers, so as to not create copies
	struct th_args_pm *args = malloc(sizeof(struct th_args_pm));
	if(args == NULL){
		fprintf(stderr,"[ERROR][process_manager] There was an error executing process manager with id %d\n", id);
		return -1;
	}
	printf("[OK][process_manager] Process with id %d waiting to produce %d elements\n", id, items_to_produce);

	//prepare the struct that will be sent to produce/consume
	args->belt_id = id;
	args->to_produce = items_to_produce;
	args->is_producer = 1;
	args->q = &q;
	
	struct th_args_pm *cons_args = malloc(sizeof(struct th_args_pm));
	if(args == NULL){
		fprintf(stderr,"[ERROR][process_manager] There was an error executing process manager with id %d\n", id);
		return -1;
	}
	printf("[OK][process_manager] Process with id %d waiting to produce %d elements\n", id, items_to_produce);

	//prepare the struct that will be sent to produce/consume
	cons_args->belt_id = id;
	cons_args->to_produce = items_to_produce;
	cons_args->is_producer = 1;
	cons_args->q = &q;
	printf("[OK][process_manager] process_manager with id %d waiting to produce %d elements\n",id,items_to_produce );
	int rp, rc; // return code of thread creation (if not 0, smth is wrong)
	rp=pthread_create(&threads[0],NULL, Produce, args);
	
	rc=pthread_create(&threads[1],NULL, Consume, cons_args);
	if(rp || rc){
		fprintf(stderr,"[ERROR][process_manager] There was an error executing process manager with id %d\n", id);
	}
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	printf("[OK][process_manager] process_manager with id %d has produced %d elements \n",id, items_to_produce);
	queue_destroy(&q);
	
   	return(0);
}
