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
int main (int argc, const char * argv[] ){
	struct belt_info {
		int id;
		int size;
		int items;
	};

	int* status;
	int id, size, items;
	FILE* f = fopen(argv, "r");
	if (f == NULL) {
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		return -1;
	}

	int num_belts;
	int count =0;
	
	fscanf(f, "%d", &num_belts);
	struct belt_info belts[num_belts];

	// we read 3 by three
	while (fscanf(f, "%d %d %d", &id, &size, &items)==3){
		belts[count].id = id;
		belts[count].size = size;
		belts[count].items = items;
		count++;
	}


	// int process_manager (int id, int belt_size, int items_to_produce )
	// make a semaphore of upto "max_concurrent_belts"and inside call process_manager?
	// make n threads (as many as existing belts)
	//int sem_init(sem_t *sem, int pshared, unsigned int value);--> value (how many threads at once)
	//sem_wait(&semaphore)-->decreases sem by 1
	//sem_post(&study_rooms) --> increases sem by 1
	// sem_destroy(&semaphore)
	sem_t semaphore;
	
	

	return 0;
}
