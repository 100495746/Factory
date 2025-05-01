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

#define NUM_THREADS 2

struct th_args_pm{
		int belt_id;
		int to_produce;
		int is_producer;
	};

//Thread function
/*void *PrintHello(void *threadid)
{
   	long tid;
   	tid = (long)threadid;
   	printf("Hello World! It's me, thread #%ld!\n", tid);
	printf("Thread #%ld ends\n", tid);
   	pthread_exit(0);
}*/

void *Produce(void *arg){
	struct th_args_pm *args = (struct th_args_pm*) arg;
	for (int i = 0; i < args->to_produce; i++) {
		struct element elem;
		elem.num_edition = i;
		elem.id_belt = args->belt_id;
		elem.last = (i == args->to_produce - 1) ? 1 : 0; // last if there are no more
		queue_put(&elem);
	}
	free(arg); 
	pthread_exit(NULL);

}

void *Consume(void *arg){
	struct th_args_pm *args = (struct th_args_pm*) arg;
	for (int i = 0; i < args->to_produce; i++) {
		struct element* elem = queue_get();
	}

	free(arg);
	pthread_exit(NULL);
}


int process_manager (int id, int belt_size, int items_to_produce ){
	pthread_t threads[NUM_THREADS]; //1 prod, 1 cons
   	long t; // numbers the thread (thread id)
	if (id <= 0 || belt_size <= 0 || items_to_produce <= 0) {
		fprintf(stderr, "[ERROR][process_manager] Arguments not valid with id %d\n", id);
		return -1;
	}

	

	//using the definded struct/function in queue.c
	//struct element * elemento = NULL;
	//queue_empty();	
	
	struct th_args_pm *prod_args = malloc(sizeof(struct th_args_pm));
	if(prod_args == NULL){
		printf(stderr,"[ERROR][process_manager] There was an error executing process manager with id %d", id);
		return -1;
	}
	printf("[OK][process_manager] Process");
	prod_args->belt_id = id;
	prod_args->to_produce = items_to_produce;
	prod_args->is_producer = 1;

	struct th_args_pm *cons_args = malloc(sizeof(struct th_args_pm));
	if(cons_args == NULL){
		printf(stderr,"[ERROR][process_manager] There was an error executing process manager with id %d", id);
		return -1;
	}
	cons_args->belt_id = id;
	cons_args->to_produce = items_to_produce;
	cons_args->is_producer = 0;
	printf("[OK][process_manager] process_manager with id %d waiting to produce %d elements");
	if(queue_init(belt_size) !=0){
		printf(stderr,"[ERROR][process_manager] There was an error executing process manager with id %d", id);
		return -1;
	};

	printf("[OK][process_manager] Belt with id %d has been created with a maximum of %d threads", id, items_to_produce);
	int rp, rc; // return code of thread creation (if not 0, smth is wrong)
	rp=pthread_create(&threads[0],NULL, Produce, prod_args);
	rc=pthread_create(&threads[1],NULL, Consume, cons_args);
	if(rp || rc){
		printf(stderr,"[ERROR][process_manager] There was an error executing process manager with id %d", id);
	}
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	printf("[OK][process_manager] process_manager with id %d has produced %d elements",id, items_to_produce);
	queue_destroy();
	

	//printf("Hello! I am the process manager and I have %d minions.\n", NUM_THREADS);

	/*for(t=0;t<NUM_THREADS;t++){
     		//printf("Creating thread %ld\n", t);
		
		launching a thread
     		rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
     		
		if (rc){
       			printf("ERROR; return code from pthread_create() is %d\n", rc);
       			exit(-1);
       		}
     	}

	int i = 0;
	for(i = 0; i < NUM_THREADS; i++)
	{	
		pthread_join(threads[i], NULL);
	}
	printf("Fin process manager\n");*/

   	/* Last thing that main() should do */
   	return(0);
}
