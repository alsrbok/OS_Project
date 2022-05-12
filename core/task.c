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

    printf("sblock_start: %p\n", sblock_start);
    printf("sblock_size: %d\n", sblock_size);
    addr_t stack_base = (addr_t)((int32u_t*)sblock_start - sblock_size);
    printf("stack_base: %p\n", stack_base);
    addr_t stack_pointer = _os_create_context(stack_base, sblock_size, entry, arg);


    (*task).stack_pointer = stack_pointer;
    (*task).priority = priority;
    

    _os_node_t *new_node = malloc(sizeof(_os_node_t));
    new_node -> ptr_data = stack_pointer;
    new_node -> priority = priority;

    _os_add_node_priority(_os_ready_queue, new_node);
    printf("result ready queue : %p\n", (*_os_ready_queue));
    printf("result ready queue prev : %p\n", (*_os_ready_queue)->previous);
    printf("result ready queue next : %p\n", (*_os_ready_queue)->next);
    
    return 0;
}

int32u_t eos_destroy_task(eos_tcb_t *task) {
}

void eos_schedule() {
    static int initial = 0;

    if(initial != 0) {

        addr_t val = _os_save_context();
        if(val == NULL) {return;}
        else{

            (*_os_ready_queue)->ptr_data = val;
            *_os_ready_queue = (*_os_ready_queue)->next;

           
            _os_add_node_priority(_os_ready_queue, (*_os_ready_queue)->previous);    

            (*_os_ready_queue)->previous->previous = \
                (*_os_ready_queue)->previous->next;
            (*_os_ready_queue)->previous->next = (*_os_ready_queue);
    
            _os_remove_node(_os_ready_queue, (*_os_ready_queue)->previous);
           
            (*_os_ready_queue)->next->next = (*_os_ready_queue)->previous;
            (*_os_ready_queue)->next->previous = (*_os_ready_queue);
           
            _os_restore_context((*_os_ready_queue)->ptr_data);
           
        }
    }
    else{
        initial++;
        _os_restore_context((*_os_ready_queue)->ptr_data);
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
}

int32u_t eos_get_period(eos_tcb_t *task) {
}

int32u_t eos_suspend_task(eos_tcb_t *task) {
}

int32u_t eos_resume_task(eos_tcb_t *task) {
}

void eos_sleep(int32u_t tick) {
}

void _os_init_task() {
    PRINT("initializing task module.\n");

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
} 
