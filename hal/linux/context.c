#include <core/eos.h>
#include <core/eos_internal.h>
#include "emulator_asm.h"

typedef struct _os_context {
	/* low address */
	int32u_t edi;
	int32u_t esi;
	int32u_t ebx;
	int32u_t edx;
	int32u_t ecx;
	int32u_t eax;
	int32u_t _eflags_val;
	int32u_t eip;
	/* high address */	
} _os_context_t;

void print_context(addr_t context) {
	if(context == NULL) {return;}
	_os_context_t *ctx = (_os_context_t *)context;
	//PRINT("reg1  =0x%x\n", ctx->reg1);
	//PRINT("reg2  =0x%x\n", ctx->reg2);
	//PRINT("reg3  =0x%x\n", ctx->reg3);
	//...
}

addr_t _os_create_context(addr_t stack_base, size_t stack_size, void (*entry)(void *), void *arg) {

	int32u_t* stack_top = (int32u_t*)stack_base + stack_size;
	*stack_top = arg;
	stack_top--;

	*stack_top = NULL;
 	stack_top--;

	*stack_top = entry;
	//printf("entry_val : %p\n", entry);
	stack_top--;

	*stack_top = 1;
	stack_top--;

	*stack_top = NULL;
 	stack_top--;
	*stack_top = NULL;
 	stack_top--;
	*stack_top = NULL;
 	stack_top--;
	*stack_top = NULL;
 	stack_top--;
	*stack_top = NULL;
 	stack_top--;
	*stack_top = NULL;

	//printf("final top of stack: %p\n", stack_top);
	
	return (addr_t)stack_top;
	
}

void _os_restore_context(addr_t sp) {

	__asm__ __volatile__(
		"movl %0, %%esp	\n\t"
		"pop %%edi	\n\t"
		"pop %%esi	\n\t"
		"pop %%ebx	\n\t"
		"pop %%edx	\n\t"
		"pop %%ecx	\n\t"
		"pop %%eax	\n\t"
		"pop _eflags	\n\t"
		"ret		\n\t"
		:: "m"(sp));
}


addr_t _os_save_context() {

	int32u_t temp_eax;
	__asm__ __volatile__(
		
		"cmp %%ebp, %%esp	\n\t"
		"jne L1			\n\t"
		"movl $0, %%eax		\n\t"
		"pop %%ebp		\n\t"
		"ret			\n\t"
		
		"L1:			\n\t"

		"movl %%eax, %0		\n\t"
		"movl (%%ebp), %%eax	\n\t"		// compute eax = *(ebp)
		"addl $4, %%eax 	\n\t"		// compute eax = *(ebp)+4
		//"movl (%%eax), %%eax 	\n\t"		// compute eax = *(*(ebp)+4)
		//"addl $4, %%eax 	\n\t"		// compute eax = *(*(ebp)+4)+4
		"push (%%eax)		\n\t"			//save eip from *(*(*(ebp)+4)+4)
		"push _eflags		\n\t" 	//push _eflags
		"push %1		\n\t"	//push original eax
		
		"push %%ecx		\n\t"
		"push %%edx		\n\t"
		"push %%ebx		\n\t"
		"push %%esi		\n\t"
		"push %%edi		\n\t"
		"movl %%esp, %%eax	\n\t"	//save esp as return value
		"push 4(%%ebp)		\n\t"			//save eip from *(ebp+4)
		"push (%%ebp)		\n\t"
		"movl %%esp, %%ebp	\n\t"
		"leave			\n\t"
		"ret			\n\t"
		
		: "=m"(temp_eax)
		: "m"(temp_eax));

	

}

