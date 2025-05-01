#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

struct th_args_pm{
		int belt_id;
		int to_produce;
		int is_producer;
		struct queue *q;
	};
int process_manager(int id, int belt_size, int items_to_produce);

#endif