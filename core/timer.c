/********************************************************
 * Filename: core/timer.c
 *
 * Author: wsyoo, RTOSLab. SNU.
 * 
 * Description: 
 ********************************************************/
#include <core/eos.h>

static eos_counter_t system_timer;

int8u_t eos_init_counter(eos_counter_t *counter, int32u_t init_value) {
	counter->tick = init_value;
	counter->alarm_queue = NULL;
	return 0;
}

void eos_set_alarm(eos_counter_t* counter, eos_alarm_t* alarm, int32u_t timeout, void (*entry)(void *arg), void *arg) {
	if(timeout == 0 || entry == NULL){
		_os_remove_node(&(counter->alarm_queue), &(alarm->alarm_queue_node));
		return;
	}
	else{	
		//PRINT("eos_set_alarm else\n");
		alarm->timeout = timeout;
		alarm->handler = entry;
		alarm->arg = arg;
		(alarm->alarm_queue_node).ptr_data = alarm;
		(alarm->alarm_queue_node).priority = timeout;
		(alarm->alarm_queue_node).previous = NULL;
		(alarm->alarm_queue_node).next = NULL;
		//PRINT("counter alarm queue address : %p\n", (counter->alarm_queue));
		//PRINT("alarm_queue_node address : %p\n", &(alarm->alarm_queue_node));
		_os_add_node_priority(&(counter->alarm_queue), &(alarm->alarm_queue_node));
		//PRINT("counter alarm queue address : %p\n", (counter->alarm_queue));
		//PRINT("counter alarm queue node addresss : %p\n", (counter->alarm_queue->ptr_data));
	}
}

eos_counter_t* eos_get_system_timer() {
	return &system_timer;
}

void eos_trigger_counter(eos_counter_t* counter) {
	eos_alarm_t* check_alarm = (counter->alarm_queue->ptr_data);
	(counter->tick)++;
	PRINT("tick\n");
	PRINT("time : %d\n",counter->tick);

	//PRINT("check alarm node address : %p\n", check_alarm);
	//PRINT("counter alarm queue address : %p\n", (counter->alarm_queue));
	//PRINT("check alarm timeout: %d\n", check_alarm->timeout);
	//PRINT("counter tick %d\n", counter->tick);
	if(check_alarm->timeout <= counter->tick){
		//PRINT("check alarm timeout: %d\n", check_alarm->timeout);
		//PRINT("time : %d\n",counter->tick);
		//_os_remove_node(&(counter->alarm_queue), &(check_alarm->alarm_queue_node));
		eos_set_alarm(counter, check_alarm, 0, NULL, NULL);
		if(check_alarm->handler != NULL){
			//PRINT("handler is not NULL\n");
			check_alarm->handler(check_alarm->arg);
		}
	}

}

/* Timer interrupt handler */
static void timer_interrupt_handler(int8s_t irqnum, void *arg) {
	/* trigger alarms */
	eos_trigger_counter(&system_timer);
}

void _os_init_timer() {
	eos_init_counter(&system_timer, 0);

	/* register timer interrupt handler */
	eos_set_interrupt_handler(IRQ_INTERVAL_TIMER0, timer_interrupt_handler, NULL);
}
