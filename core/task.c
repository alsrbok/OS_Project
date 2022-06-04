/********************************************************
 * Filename: core/task.c
 *
 * Author: parkjy, RTOSLab. SNU.
 *
 * Description: task management.
 ********************************************************/
#include <core/eos.h>

#define READY        1
#define RUNNING        2
#define WAITING        3

/*
 * Queue (list) of tasks that are ready to run.
 */
static _os_node_t *_os_ready_queue[LOWEST_PRIORITY + 1];

/*
 * Pointer to TCB of running task
 */
static eos_tcb_t *_os_current_task;

int32u_t eos_create_task(eos_tcb_t *task, addr_t sblock_start, size_t sblock_size, void (*entry)(void *arg), void *arg, int32u_t priority) {
	PRINT("task: 0x%x, priority: %d\n", (int32u_t)task, priority);

	//addr_t stack_base = (addr_t)((int32u_t*)sblock_start - sblock_size);
	addr_t stack_pointer = _os_create_context(sblock_start, sblock_size, entry, arg);
	(*task).stack_pointer = stack_pointer;
	(*task).priority = priority;
	(*task).state = READY;
	(*task).node.ptr_data = task;
	(*task).node.previous = NULL;
	(*task).node.next = NULL;
	(*task).node.priority = priority;
	_os_add_node_priority(&(_os_ready_queue[priority]), &((*task).node));
	//PRINT("task : %p\n", task);
	//PRINT("task's node : %p\n", (*task).node);
	//PRINT("task's node ptr_data: %p\n", (*task).node.ptr_data);
	_os_set_ready(priority);

   	return 0;
}

int32u_t eos_destroy_task(eos_tcb_t *task) {
}

void eos_schedule() {

    static int initial = 0;
    //static int task = 0;

    if(initial != 0) {
	//PRINT("eos_schedule if\n");
	int32u_t highest_priority = _os_get_highest_priority();
	if(highest_priority != 0){
		//addr_t val = _os_save_context();
		//if(task < 3){_os_current_task->saved_point = val; task++;}
		//PRINT("%p task's saved stack pointer address : %p\n", _os_current_task, val);
		//if(val == NULL) {return;}
		//else{
			//PRINT("eos_schedule ifelse\n");
			//PRINT("os_current task address : %p\n", _os_current_task);
		
			//_os_current_task->stack_pointer = val;	

			//PRINT("highest_priority : %d\n", highest_priority);
		

			_os_current_task->state = WAITING;

			eos_tcb_t* next_task = _os_ready_queue[highest_priority]->ptr_data;
			//PRINT("next task address : %p\n", next_task);
			_os_remove_node(&(_os_ready_queue[highest_priority]),
					_os_ready_queue[highest_priority]);
			_os_unset_ready(highest_priority);
			next_task->state = RUNNING;
			_os_current_task = next_task;
			//PRINT("%p task's stack top pointer address : %p\n", _os_current_task, _os_current_task->stack_pointer);
			_os_restore_context(_os_current_task->stack_pointer);
			
		
		//}
		

        }
	else{
	
		eos_counter_t* counter = eos_get_system_timer();
		if(counter->alarm_queue != NULL){
			eos_alarm_t* check_alarm = (counter->alarm_queue->ptr_data);
			if(check_alarm->timeout <= counter->tick){
			//PRINT("check alarm timeout: %d\n", check_alarm->timeout);
			//PRINT("time : %d\n",counter->tick);
			_os_remove_node(&(counter->alarm_queue), &(check_alarm->alarm_queue_node));
			//eos_set_alarm(counter, check_alarm, 0, NULL, NULL);
				if(check_alarm->handler != NULL){
					//PRINT("handler is not NULL\n");
					check_alarm->handler(check_alarm->arg);
				}
			}
		} 
	eos_schedule();
	}
    }
    else{
        initial++;
	//PRINT("eos_schedule else\n");
	int32u_t highest_priority = _os_get_highest_priority();
	//PRINT("highest_priority : %d\n", highest_priority);
	_os_current_task = _os_ready_queue[highest_priority]->ptr_data;
	//PRINT("priority 1 address: %p\n", _os_ready_queue[1]->ptr_data);	
	//PRINT("next task address : %p\n", _os_ready_queue[highest_priority]->ptr_data);
	_os_remove_node(&(_os_ready_queue[highest_priority]), _os_ready_queue[highest_priority]);
	//PRINT("priority 1 address: %p\n", _os_ready_queue[1]);
	_os_unset_ready(highest_priority);
	//PRINT("os ready queue[highest] : %d\n", _os_ready_queue[highest_priority]);
	_os_current_task -> state = RUNNING;
        _os_restore_context(_os_current_task->stack_pointer);
    }
}   

eos_tcb_t *eos_get_current_task() {
    return _os_current_task;
}

void eos_change_priority(eos_tcb_t *task, int32u_t priority) {
}

int32u_t eos_get_priority(eos_tcb_t *task) {
}

void eos_set_period(eos_tcb_t *task, int32u_t period){
	(*task).period = period;
}

int32u_t eos_get_period(eos_tcb_t *task) {
}

int32u_t eos_suspend_task(eos_tcb_t *task) {
}

int32u_t eos_resume_task(eos_tcb_t *task) {
}

void eos_sleep(int32u_t tick) {
	eos_counter_t *sys_timer = eos_get_system_timer();
	eos_alarm_t *new_alarm = (eos_alarm_t*)malloc(sizeof(eos_alarm_t));
	int32u_t timeout = _os_current_task->period + sys_timer->tick;
	//PRINT("_os_current_task address : %p\n", _os_current_task);
	//PRINT("_os_current_task->period : %d\n", _os_current_task->period);
	//PRINT("sys_timer->tick : %d\n", sys_timer->tick);
	//PRINT("timeout : %d\n", timeout);
	_os_current_task->stack_pointer = _os_save_context();
	eos_set_alarm(sys_timer, new_alarm, timeout, _os_wakeup_sleeping_task, _os_current_task);
	if(_os_save_context() == NULL) return;
	else eos_schedule();
}

void _os_init_task() {

    /* init current_task */
    _os_current_task = NULL;

    /* init multi-level ready_queue */
    int32u_t i;
    for (i = 0; i < LOWEST_PRIORITY; i++) {
        _os_ready_queue[i] = NULL;
    }
}

void _os_wait(_os_node_t **wait_queue) {
}

void _os_wakeup_single(_os_node_t **wait_queue, int32u_t queue_type) {
}

void _os_wakeup_all(_os_node_t **wait_queue, int32u_t queue_type) {
}

void _os_wakeup_sleeping_task(void *arg) {
	//PRINT("os_wakeup is start\n");
	eos_tcb_t* sleeping_task = (eos_tcb_t*)arg;
	sleeping_task->state = READY;
	sleeping_task->node.ptr_data = sleeping_task;
	sleeping_task->node.previous = NULL;
	sleeping_task->node.next = NULL;
	int32u_t priority = sleeping_task->priority;
	_os_add_node_priority(&(_os_ready_queue[priority]), &(sleeping_task->node));
	_os_ready_queue[priority]->ptr_data = sleeping_task;
	_os_set_ready(priority);
	//PRINT("sleeping task : %p\n", sleeping_task);
	//PRINT("sleeping task period: %p\n", sleeping_task->period);
	//PRINT("result of add node tail address: %p\n", _os_ready_queue[priority]->ptr_data);
	eos_schedule();
} 
