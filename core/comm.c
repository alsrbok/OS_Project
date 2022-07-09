/********************************************************
 * Filename: core/comm.c
 *  
 * Author: jtlim, RTOSLab. SNU.
 * 
 * Description: message queue management. 
 ********************************************************/
#include <core/eos.h>

void eos_init_mqueue(eos_mqueue_t *mq, void *queue_start, int16u_t queue_size, int8u_t msg_size, int8u_t queue_type) {
	mq->queue_size = queue_size;
	mq->msg_size = msg_size;
	mq->queue_start = queue_start;
	mq->front = queue_start;
	mq->rear = queue_start;
	mq->queue_type = queue_type;
	mq->putsem = (eos_semaphore_t*)malloc(sizeof(eos_semaphore_t));
	mq->getsem = (eos_semaphore_t*)malloc(sizeof(eos_semaphore_t));
	eos_init_semaphore((mq->putsem), queue_size, queue_type);
	eos_init_semaphore((mq->getsem), 0, queue_type);
}

int8u_t eos_send_message(eos_mqueue_t *mq, void *message, int32s_t timeout) {
	eos_acquire_semaphore((mq->putsem), timeout);
	for(int i = 0; i<mq->msg_size; i++){
		*((int8u_t*)(mq->rear) + i) = *((int8u_t*)message + i);
	}
	mq->rear = (int8u_t*)(mq->rear) + mq->msg_size;
	PRINT("saved message in front : %s\n", mq->front);
	eos_release_semaphore((mq->getsem));
	//PRINT("Bye\n");
	//eos_schedule();
}

int8u_t eos_receive_message(eos_mqueue_t *mq, void *message, int32s_t timeout) {

	PRINT("mq : %p\n", mq);
	PRINT("get sem semaphore : %p\n", (mq->getsem));
	PRINT("put sem semaphore : %p\n", (mq->putsem));

	//__asm__ __volatile__("movl %0, %%esi	\n\t":: "m"(mq));

	eos_acquire_semaphore((mq->getsem), timeout);
	PRINT("Please\n");
	//PRINT("%p semaphore count : %d\n", (mq->getsem), (mq->getsem)->count);
	//__asm__ __volatile__("movl %%edi, %0\n\t":"=m"(mq));
	//mq =  0x8067f5c;
	//eos_acquire_semaphore((mq->getsem), timeout);
	//PRINT("flag = %d\n",flag);
	PRINT("mq : %p\n", mq);
	PRINT("msg size : %d\n",mq->msg_size);
	PRINT("get sem semaphore : %p\n", (mq->getsem));
	PRINT("saved message in front : %s\n", mq->front);
	for(int i = 0; i<mq->msg_size; i++){
		*((int8u_t*)message + i) = *((int8u_t*)(mq->front) + i);
	}
	PRINT("receive message: %s\n", message);
	mq->front = (int8u_t*)(mq->front) + mq->msg_size;
	//PRINT("%p semaphore count : %d\n", &(mq->putsem), mq->getsem->count);
	PRINT("put sem semaphore : %p\n", (mq->putsem));
	eos_release_semaphore((mq->putsem));
	return;
}
