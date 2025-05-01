/*
 *
 * factory_manager.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <pthread.h>
#include "process_manager.c"
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
void *belt_wrapper(void *arg){
	struct belt_info *belt = (struct belt_info*) arg;
	process_manager(belt->id, belt->size, belt->items);
	return NULL;
}

int main (int argc, const char * argv[] ){


	int* status;
	int id, size, items;
	FILE* f = fopen(argv, "r");
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
		old_belts[count].id = id;
		old_belts[count].size = size;
		old_belts[count].items = items;
		count++;
		// dynamic memory
		struct temporary*  new_belts = malloc(count * sizeof(struct belt_info));
		memcpy(new_belts, old_belts, count*sizeof(struct belt_info));
		free(old_belts);
		old_belts = new_belts;
	}
	//free(new_belts)

	// int process_manager (int id, int belt_size, int items_to_produce )
	// make a semaphore of upto "max_concurrent_belts"and inside call process_manager?
	// make n threads (as many as existing belts)
	//int sem_init(sem_t *sem, int pshared, unsigned int value);--> value (how many threads at once)
	//sem_wait(&semaphore)-->decreases sem by 1
	//sem_post(&semaphore) --> increases sem by 1
	// sem_destroy(&semaphore) --> after all use of semaphore is done
	sem_t semaphore;
	pthread_t threads[count];
	for (int i =0; i<count; i++){
		pthread_create(threads[i], NULL, process_manager,  old_belts);
	}

	for (int i =0; i<count; i++){
		
	}


	

	return 0;
}
