/********************************************************
 * Filename: core/sync.c
 * 
 * Author: wsyoo, RTOSLab. SNU.
 * 
 * Description: semaphore, condition variable management.
 ********************************************************/
#include <core/eos.h>

void eos_init_semaphore(eos_semaphore_t *sem, int32u_t initial_count, int8u_t queue_type) {
	/* initialization */
	//sem = (eos_semaphore_t*)malloc(sizeof(eos_semaphore_t));
	(*sem).count = initial_count;
	(*sem).queue_type = queue_type;
}

int32u_t eos_acquire_semaphore(eos_semaphore_t *sem, int32s_t timeout) {
	//addr_t save_point = 0;
	//__asm__ __volatile__(
	//	"movl (%%ebp), %%eax	\n\t"		// compute eax = *(ebp)
	//	"addl $4, %%eax 	\n\t"		// compute eax = *(ebp)+4
	//	"movl %%eax, %0 	\n\t"
	//	:"=m"(save_point));
	int flag = 0;
	__asm__ __volatile__("movl %0, %%esi	\n\t":: "m"(sem));
	while(1){
		//PRINT("acquire semaphore\n");
		PRINT("%p semaphore \n", sem);
		if(sem->count > 0){
			PRINT("if\n");
			(sem->count) = (sem->count)-1;
			PRINT("%p semaphore count : %d\n", sem, sem->count);
			//if(flag==1){
			//	__asm__ __volatile__(
			//	"movl %0, %%esp		\n\t"
			//	"ret			\n\t"
			//	:: "m"(save_point));
			//flag = 2;
			//return 0;
			//}
			//else return 0;
			return 0;

		}
		else{
			if(timeout == -1) return -1;
			else if(timeout == 0){
				//PRINT("timeout = 0\n");
				//_os_node_t *head = sem->wait_queue;
				if(sem->queue_type == 0) {
					PRINT("queue type FIFO\n");
					_os_add_node_tail(&((*sem).wait_queue), 
						&(eos_get_current_task()->node));
				}
				else {
					PRINT("queue type priority\n");
					_os_add_node_priority(&((*sem).wait_queue), 
						&(eos_get_current_task()->node));
				}
				PRINT("%p semaphore wait_queue : %p\n", sem, sem->wait_queue);
				//addr_t val = _os_save_context();
				//PRINT("%p current task's saved stack pointer address : %p\n", eos_get_current_task(), val);
				//eos_get_current_task()->stack_pointer = val;
				//PRINT("priority %d 's stack pointer = %p\n", eos_get_current_task()->priority, eos_get_current_task()->stack_pointer);
				eos_schedule();
				__asm__ __volatile__("movl %%esi, %0	\n\t":"=m"(sem));
				PRINT("Restart\n");
				flag = 1;
			}
			else{	
				if(sem->queue_type == 0) {
					_os_add_node_tail(&(sem->wait_queue), 
						&(eos_get_current_task()->node));
				}
				else {
					_os_add_node_priority(&(sem->wait_queue), 
						&(eos_get_current_task()->node));
				}
				eos_counter_t *sys_timer = eos_get_system_timer();
				eos_alarm_t *new_alarm = (eos_alarm_t*)malloc(sizeof(eos_alarm_t));
				int32u_t new_timeout = timeout + sys_timer->tick;
				eos_set_alarm(sys_timer, new_alarm, new_timeout, 						_os_wakeup_sleeping_task, eos_get_current_task());
				//eos_get_current_task()->stack_pointer = _os_save_context();
				eos_schedule();
			}
		}
	}
}

void eos_release_semaphore(eos_semaphore_t *sem) {
	(sem->count) = (sem->count)+1;
	PRINT("%p semaphore count : %d\n", sem, sem->count);
	if(sem->wait_queue != NULL){
		//PRINT("release semaphore\n");
		//PRINT("%p semaphore wait_queue : %p\n", sem, sem->wait_queue);
		_os_wakeup_single(&((*sem).wait_queue), sem->queue_type);
		//eos_tcb_t* waiting_task = sem->wait_queue->ptr_data;
		//_os_remove_node(&(sem->wait_queue), &(waiting_task->node));
		//int32u_t priority = waiting_task->priority;
		//_os_add_node_priority(&(_os_ready_queue[priority]), &(waiting_task->node));
		//_os_ready_queue[priority]->ptr_data = waiting_task;
		//_os_set_ready(priority);
		//eos_schedule();
	}
}

void eos_init_condition(eos_condition_t *cond, int32u_t queue_type) {
	/* initialization */
	cond->wait_queue = NULL;
	cond->queue_type = queue_type;
}

void eos_wait_condition(eos_condition_t *cond, eos_semaphore_t *mutex) {
	/* release acquired semaphore */
	eos_release_semaphore(mutex);
	/* wait on condition's wait_queue */
	_os_wait(&cond->wait_queue);
	/* acquire semaphore before return */
	eos_acquire_semaphore(mutex, 0);
}

void eos_notify_condition(eos_condition_t *cond) {
	/* select a task that is waiting on this wait_queue */
	_os_wakeup_single(&cond->wait_queue, cond->queue_type);
}
