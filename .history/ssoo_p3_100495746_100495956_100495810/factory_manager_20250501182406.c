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
	// we make sure only a mximum number may enter at a time (with the semaphore)
	
	sem_wait(args->semaphore);
	int result;
	//start a new process
	result = process_manager(args->belt.id, args->belt.size, args->belt.items);
	if (result!=0){
		fprintf(stderr, "[ERROR][factory_manager] Process with id %d has finished with errors.\n",args->belt.id );
	}
	sem_post(args->semaphore);
	free(arg);
	return NULL;
}

int main (int argc, const char * argv[] ){
	// int* status; // i dont know if this is needed, ill check later
	FILE* f = fopen(argv[1], "r");
	if (f == NULL) {
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		return -1;
	}
	// we only know the size after reading the file, so we need to make it dynamic

	int max_concurrent;
	if (fscanf(f, "%d", &max_concurrent) != 1 || max_concurrent <= 0) {
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		fclose(f);
		return -1;
	}

	int values[300]; //buffer
	int v = 0;
	while (fscanf(f, "%d", &values[v]) == 1) {
		v++;
	}

	if (v % 3 != 0) {
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		fclose(f);
		return -1;
	}

	int num_trios = v / 3;
	if (num_trios > max_concurrent) {
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		fclose(f);
		return -1;
	}

// Ahora ya puedes usar values[] para rellenar tu array de belts:
	struct belt_info* old_belts = malloc(num_trios * sizeof(struct belt_info));
	for (int i = 0; i < num_trios; i++) {
		int id = values[i*3];
		int size = values[i*3 + 1];
		int items = values[i*3 + 2];
		if (id <= 0 || size <= 0 || items <= 0) {
			fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
			fclose(f);
			free(old_belts);
			return -1;
		}
		old_belts[i].id = id;
		old_belts[i].size = size;
		old_belts[i].items = items;
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
	pthread_t threads[num_trios];
	
	for (int i =0; i<num_trios; i++){
		struct th_args_fm* args = malloc(sizeof(struct th_args_fm));
		// each belt, one thread, but same semaphore
		args->belt = old_belts[i];
		args->semaphore = &semaphore;
		pthread_create(&threads[i], NULL, belt_wrapper, args);
		
	}
	
	for (int i =0; i<num_trios; i++){
		//end the threads
		pthread_join(threads[i],NULL );
		printf("[OK][factory_manager] process_manager with id %d has finished.\n", old_belts[i].id);
	}
	free(old_belts);
	sem_destroy(&semaphore);
	printf("[OK][factory_manager] Finishing.\n");

	return 0;
}
