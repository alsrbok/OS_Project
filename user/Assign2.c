#include <core/eos.h>
#define STACK_SIZE 8096

int8u_t stack1[STACK_SIZE];
int8u_t stack2[STACK_SIZE];
eos_tcb_t tcb1;
eos_tcb_t tcb2;

void print_number(){
	int i = 0;
	while(++i){
		printf("%d\n", i);
		eos_schedule();
		if(i==20) {i=0;}
	}
}

void print_alphabet(){
	int i = 96;
	while(++i){
		printf("%c\n", i);
		eos_schedule();
		if(i==122) {i=96;}
	}
}

void eos_user_main(){
	printf("task1 entry : %p\n", print_number);
	printf("task2 entry : %p\n", print_alphabet);
	eos_create_task(&tcb1, stack1, STACK_SIZE, print_number, NULL, 0);
	eos_create_task(&tcb2, stack2, STACK_SIZE, print_alphabet, NULL, 0);
}
