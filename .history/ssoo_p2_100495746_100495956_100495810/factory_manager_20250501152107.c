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

		raulas@raul-macbookair:~/GIT/Factory/ssoo_p2_100495746_100495956_100495810$ valgrind ./factory test.txt
==38578== Memcheck, a memory error detector
==38578== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==38578== Using Valgrind-3.24.0 and LibVEX; rerun with -h for copyright info
==38578== Command: ./factory test.txt
==38578== 
==38578== Invalid write of size 4
==38578==    at 0x400F74: main (factory_manager.c:78)
==38578==  Address 0x4a9d2fc is 0 bytes after a block of size 12 alloc'd
==38578==    at 0x48854F0: malloc (vg_replace_malloc.c:446)
==38578==    by 0x400FEB: main (factory_manager.c:83)
==38578== 
==38578== Invalid write of size 4
==38578==    at 0x400F9C: main (factory_manager.c:79)
==38578==  Address 0x4a9d300 is 4 bytes after a block of size 12 alloc'd
==38578==    at 0x48854F0: malloc (vg_replace_malloc.c:446)
==38578==    by 0x400FEB: main (factory_manager.c:83)
==38578== 
==38578== Invalid write of size 4
==38578==    at 0x400FC4: main (factory_manager.c:80)
==38578==  Address 0x4a9d304 is 8 bytes after a block of size 12 alloc'd
==38578==    at 0x48854F0: malloc (vg_replace_malloc.c:446)
==38578==    by 0x400FEB: main (factory_manager.c:83)
==38578== 
==38578== Invalid read of size 8
==38578==    at 0x48910C0: __GI_memcpy (vg_replace_strmem.c:1151)
==38578==    by 0x401013: main (factory_manager.c:84)
==38578==  Address 0x4a9d300 is 4 bytes after a block of size 12 alloc'd
==38578==    at 0x48854F0: malloc (vg_replace_malloc.c:446)
==38578==    by 0x400FEB: main (factory_manager.c:83)
==38578== 
==38578== Invalid read of size 8
==38578==    at 0x4891068: __GI_memcpy (vg_replace_strmem.c:1151)
==38578==    by 0x401013: main (factory_manager.c:84)
==38578==  Address 0x4a9d358 is 0 bytes after a block of size 24 alloc'd
==38578==    at 0x48854F0: malloc (vg_replace_malloc.c:446)
==38578==    by 0x400FEB: main (factory_manager.c:83)
==38578== 
==38578== Invalid read of size 2
==38578==    at 0x4890FC0: __GI_memcpy (vg_replace_strmem.c:1151)
==38578==    by 0x401013: main (factory_manager.c:84)
==38578==  Address 0x4a9d360 is 8 bytes after a block of size 24 alloc'd
==38578==    at 0x48854F0: malloc (vg_replace_malloc.c:446)
==38578==    by 0x400FEB: main (factory_manager.c:83)
==38578== 
==38578== Thread 3:
==38578== Conditional jump or move depends on uninitialised value(s)
==38578==    at 0x401320: process_manager (process_manager.c:64)
==38578==    by 0x400E2F: belt_wrapper (factory_manager.c:48)
==38578==    by 0x495BD67: start_thread (in /usr/lib64/libc.so.6)
==38578==    by 0x49C769B: thread_start (in /usr/lib64/libc.so.6)
==38578== 
==38578== Use of uninitialised value of size 8
==38578==    at 0x492131C: _itoa_word (in /usr/lib64/libc.so.6)
==38578==    by 0x492AD07: __printf_buffer (in /usr/lib64/libc.so.6)
==38578==    by 0x492C98F: __vfprintf_internal (in /usr/lib64/libc.so.6)
==38578==    by 0x4922507: printf (in /usr/lib64/libc.so.6)
==38578==    by 0x4013B7: process_manager (process_manager.c:80)
==38578==    by 0x400E2F: belt_wrapper (factory_manager.c:48)
==38578==    by 0x495BD67: start_thread (in /usr/lib64/libc.so.6)
==38578==    by 0x49C769B: thread_start (in /usr/lib64/libc.so.6)
==38578== 
==38578== Conditional jump or move depends on uninitialised value(s)
==38578==    at 0x4921328: _itoa_word (in /usr/lib64/libc.so.6)
==38578==    by 0x492AD07: __printf_buffer (in /usr/lib64/libc.so.6)
==38578==    by 0x492C98F: __vfprintf_internal (in /usr/lib64/libc.so.6)
==38578==    by 0x4922507: printf (in /usr/lib64/libc.so.6)
==38578==    by 0x4013B7: process_manager (process_manager.c:80)
==38578==    by 0x400E2F: belt_wrapper (factory_manager.c:48)
==38578==    by 0x495BD67: start_thread (in /usr/lib64/libc.so.6)
==38578==    by 0x49C769B: thread_start (in /usr/lib64/libc.so.6)
==38578== 
==38578== Conditional jump or move depends on uninitialised value(s)
==38578==    at 0x492B61C: __printf_buffer (in /usr/lib64/libc.so.6)
==38578==    by 0x492C98F: __vfprintf_internal (in /usr/lib64/libc.so.6)
==38578==    by 0x4922507: printf (in /usr/lib64/libc.so.6)
==38578==    by 0x4013B7: process_manager (process_manager.c:80)
==38578==    by 0x400E2F: belt_wrapper (factory_manager.c:48)
==38578==    by 0x495BD67: start_thread (in /usr/lib64/libc.so.6)
==38578==    by 0x49C769B: thread_start (in /usr/lib64/libc.so.6)
==38578== 
==38578== Conditional jump or move depends on uninitialised value(s)
==38578==    at 0x492B738: __printf_buffer (in /usr/lib64/libc.so.6)
==38578==    by 0x492C98F: __vfprintf_internal (in /usr/lib64/libc.so.6)
==38578==    by 0x4922507: printf (in /usr/lib64/libc.so.6)
==38578==    by 0x4013B7: process_manager (process_manager.c:80)
==38578==    by 0x400E2F: belt_wrapper (factory_manager.c:48)
==38578==    by 0x495BD67: start_thread (in /usr/lib64/libc.so.6)
==38578==    by 0x49C769B: thread_start (in /usr/lib64/libc.so.6)
==38578== 
[OK][process_manager] Process with id 1 waiting to produce 3 elements
==38578== Conditional jump or move depends on uninitialised value(s)
==38578==    at 0x492B61C: __printf_buffer (in /usr/lib64/libc.so.6)
==38578==    by 0x492C98F: __vfprintf_internal (in /usr/lib64/libc.so.6)
==38578==    by 0x4922507: printf (in /usr/lib64/libc.so.6)
==38578==    by 0x40144F: process_manager (process_manager.c:93)
==38578==    by 0x400E2F: belt_wrapper (factory_manager.c:48)
==38578==    by 0x495BD67: start_thread (in /usr/lib64/libc.so.6)
==38578==    by 0x49C769B: thread_start (in /usr/lib64/libc.so.6)
==38578== 
==38578== Conditional jump or move depends on uninitialised value(s)
==38578==    at 0x492B738: __printf_buffer (in /usr/lib64/libc.so.6)
==38578==    by 0x492C98F: __vfprintf_internal (in /usr/lib64/libc.so.6)
==38578==    by 0x4922507: printf (in /usr/lib64/libc.so.6)
==38578==    by 0x40144F: process_manager (process_manager.c:93)
==38578==    by 0x400E2F: belt_wrapper (factory_manager.c:48)
==38578==    by 0x495BD67: start_thread (in /usr/lib64/libc.so.6)
==38578==    by 0x49C769B: thread_start (in /usr/lib64/libc.so.6)
==38578== 
[OK][process_manager] process_manager with id 1 waiting to produce 3 elements
==38578== Conditional jump or move depends on uninitialised value(s)
==38578==    at 0x492B61C: __printf_buffer (in /usr/lib64/libc.so.6)
==38578==    by 0x492C98F: __vfprintf_internal (in /usr/lib64/libc.so.6)
==38578==    by 0x4922507: printf (in /usr/lib64/libc.so.6)
==38578==    by 0x40149B: process_manager (process_manager.c:99)
==38578==    by 0x400E2F: belt_wrapper (factory_manager.c:48)
==38578==    by 0x495BD67: start_thread (in /usr/lib64/libc.so.6)
==38578==    by 0x49C769B: thread_start (in /usr/lib64/libc.so.6)
==38578== 
==38578== Conditional jump or move depends on uninitialised value(s)
==38578==    at 0x492B738: __printf_buffer (in /usr/lib64/libc.so.6)
==38578==    by 0x492C98F: __vfprintf_internal (in /usr/lib64/libc.so.6)
==38578==    by 0x4922507: printf (in /usr/lib64/libc.so.6)
==38578==    by 0x40149B: process_manager (process_manager.c:99)
==38578==    by 0x400E2F: belt_wrapper (factory_manager.c:48)
==38578==    by 0x495BD67: start_thread (in /usr/lib64/libc.so.6)
==38578==    by 0x49C769B: thread_start (in /usr/lib64/libc.so.6)
==38578== 
[OK][process_manager] Belt with id 1 has been created with a maximum of 3 threads
[OK][process_manager] Process with id 5 waiting to produce 2 elements
[OK][process_manager] process_manager with id 5 waiting to produce 2 elements
[OK][process_manager] Belt with id 5 has been created with a maximum of 2 threads
[OK][process_manager] Process with id 2 waiting to produce 4 elements
[OK][process_manager] process_manager with id 2 waiting to produce 4 elements
[OK][process_manager] Belt with id 2 has been created with a maximum of 4 threads

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
