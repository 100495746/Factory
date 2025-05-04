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
 
 // Function declaration from process_manager.c
 int process_manager(int id, int belt_size, int items_to_produce);
 
 struct belt_info {
	 int id;
	 int size;
	 int items;
 };
 
 struct th_args_fm {
	 sem_t* semaphore;
	 struct belt_info belt;
 };
 
 void* belt_wrapper(void* arg) {
	 struct th_args_fm* args = (struct th_args_fm*) arg;
	 sem_wait(args->semaphore);
	 int result = process_manager(args->belt.id, args->belt.size, args->belt.items);
	 if (result != 0) {
		 fprintf(stderr, "[ERROR][factory_manager] Process_manager with id %d has finished with errors.\n", args->belt.id);
	 }
	 sem_post(args->semaphore);
	 free(arg);
	 return NULL;
 }
 
 int main(int argc, const char* argv[]) {
	 if (argc != 2) {
		 fprintf(stderr, "[ERROR][factory_manager] Usage: %s <config_file>\n", argv[0]);
		 return -1;
	 }
 
	 FILE* f = fopen(argv[1], "r");
	 if (f == NULL) {
		 fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		 return -1;
	 }
 
	 int max_concurrent;
	 if (fscanf(f, "%d", &max_concurrent) != 1 || max_concurrent <= 0) {
		 fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		 fclose(f);
		 return -1;
	 }
 
	 int values[300];
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
 
	 struct belt_info* old_belts = malloc(num_trios * sizeof(struct belt_info));
	 for (int i = 0; i < num_trios; i++) {
		 int id = values[i * 3];
		 int size = values[i * 3 + 1];
		 int items = values[i * 3 + 2];
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
 
	 sem_t semaphore;
	 sem_init(&semaphore, 0, max_concurrent);
	 pthread_t threads[num_trios];
 
	 for (int i = 0; i < num_trios; i++) {
		 struct th_args_fm* args = malloc(sizeof(struct th_args_fm));
		 args->belt = old_belts[i];
		 args->semaphore = &semaphore;
 
		 pthread_create(&threads[i], NULL, belt_wrapper, args);
		 printf("[OK][factory_manager] Process_manager with id %d has been created.\n", old_belts[i].id);
	 }
 
	 for (int i = 0; i < num_trios; i++) {
		 pthread_join(threads[i], NULL);
		 printf("[OK][factory_manager] Process_manager with id %d has finished.\n", old_belts[i].id);
	 }
 
	 free(old_belts);
	 sem_destroy(&semaphore);
	 printf("[OK][factory_manager] Finishing.\n");
 
	 return 0;
 }