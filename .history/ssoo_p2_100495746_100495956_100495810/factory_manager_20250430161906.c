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
	int* status;
	FILE* f = fopen()
	

	return 0;
}
