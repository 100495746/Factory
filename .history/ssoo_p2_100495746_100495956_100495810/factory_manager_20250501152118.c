/*
 *
 * factory_manager.c
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <pthread.h>
#include "process_manager.h"
//format: <max_concurrent_belts> [<belt_id> <belt_size> <num_items>]...
//ex: 4 5 5 2 1 2 3 3 5 2
/*
Which means:
    Max 4 belts at once
    Then:
        Belt 5 → size 5 → 2 items
        Belt 1 → size 2 → 3 items
        Belt 3 → size 5 → 2 items
*/

struct belt_info {
	int id;
	int size;
	int items;
};

struct th_args_fm{
		sem_t* semaphore;
		struct belt_info belt;

	};


void *belt_wrapper(void *arg){
	// prepares the arguments for process_manager and handles semaphores

	struct  th_args_fm *args = (struct th_args_fm*) arg;
	// we make sure only a mximum number may enter at a time
	
	sem_wait(args->semaphore);
	int result;
	result = process_manager(args->belt.id, args->belt.size, args->belt.items);
	if (result!=0){
		fprintf(stderr, "[ERROR][factory_manager] Process with id %d has finished with errors\n",args->belt.id );
	}
	printf("[OK][factory_manager] process_manager with id %d has been created\n", args->belt.id);
	sem_post(args->semaphore);
	free(arg);
	return NULL;
}

int main (int argc, const char * argv[] ){
	// int* status; // i dont know if this is needed, ill check later
	int id, size, items;
	FILE* f = fopen(argv[1], "r");
	if (f == NULL) {
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		return -1;
	}

	int max_concurrent;
	int count =0;
	
	fscanf(f, "%d", &max_concurrent);
	struct belt_info*  old_belts = malloc(sizeof(struct belt_info));

	// belts[max_concurrent];
	

	// we read 3 by three
	while (fscanf(f, "%d %d %d", &id, &size, &items)==3){
		
		// dynamic memory, we dont know how many belts until we read them
		struct belt_info*  new_belts = malloc(count * sizeof(struct belt_info));
		memcpy(new_belts, old_belts, count*sizeof(struct belt_info));
		free(old_belts);
		old_belts = new_belts;
		
		old_belts[count].id = id;
		old_belts[count].size = size;
		old_belts[count].items = items;
		count++;
	}
	

	// int process_manager (int id, int belt_size, int items_to_produce )
	// make a semaphore of upto "max_concurrent_belts"and inside call process_manager?
	// make n threads (as many as existing belts)
	//int sem_init(sem_t *sem, int pshared, unsigned int value);--> value (how many threads at once)
	//sem_wait(&semaphore)-->decreases sem by 1
	//sem_post(&semaphore) --> increases sem by 1
	// sem_destroy(&semaphore) --> after all use of semaphore is done
	sem_t semaphore;
	sem_init(&semaphore, 0, max_concurrent);
	pthread_t threads[count];
	
	for (int i =0; i<count; i++){
		struct th_args_fm* args = malloc(sizeof(struct th_args_fm));
		// each belt, one thread, but same semaphore
		args->belt = old_belts[i];
		args->semaphore = &semaphore;
		pthread_create(&threads[i], NULL, belt_wrapper, args);
	}
	
	for (int i =0; i<count; i++){
		pthread_join(threads[i],NULL );
		printf("[OK][factory_manager] process_manager with id %d has finished\n", old_belts[i].id);
	}
	free(old_belts);
	sem_destroy(&semaphore);
	printf("[OK][factory_manager] Finishing\n");

	return 0;
}
