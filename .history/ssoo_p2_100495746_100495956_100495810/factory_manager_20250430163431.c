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


	while (fscanf(f, "%d %d %d", &id, &size, &items)==3){
		belts[count]
		
	}
	
	

	return 0;
}
