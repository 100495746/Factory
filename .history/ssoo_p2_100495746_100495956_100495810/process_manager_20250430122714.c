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


//Thread function
void *PrintHello(void *threadid)
{
   	long tid;
   	tid = (long)threadid;
   	printf("Hello World! It's me, thread #%ld!\n", tid);
	printf("Thread #%ld ends\n", tid);
   	pthread_exit(0);
}

void *Produce(void *arg){
	struct thread_args *args = (struct thread_args*) arg;

	struct element elem = {?, args->belt_id, args->last };//??
	queue_put(elem);
	free(arg);
	pthread_exit(NULL);

}

void *Consume(void *arg){
	struct thread_args *args = (struct thread_args*) arg;
	struct element elem = {2, args->belt_id, args->last };
	queue_get(elem)
	free(&arg);
	pthread_exit(NULL);
}


int process_manager (int id, int belt_size, int items_to_produce ){
	pthread_t threads[NUM_THREADS]; //1 prod, 1 cons
   	int rc; // return code of thread creation (if not 0, smth is wrong)
   	long t; // numbers the thread (thread id)
	

	//using the definded struct/function in queue.c
	struct element * elemento = NULL;
	//queue_empty();	
	struct thread_args{
		int belt_id;
		int to_produce;
		int is_producer;
	};
	struct thread_args *prod_args = malloc(sizeof(struct thread_args));
	prod_args->belt_id = id;
	prod_args->to_produce = items_to_produce;
	prod_args->is_producer = 1;

	struct thread_args *cons_args = malloc(sizeof(struct thread_args));
	prod_args->belt_id = id;
	prod_args->to_produce = items_to_produce;
	prod_args->is_producer = 0;
	


	queue_init(belt_size);

	pthread_create(&threads[0],NULL, *Produce, &prod_args);
	pthread_create(&threads[1],NULL, *Consume, &cons_args);
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
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
