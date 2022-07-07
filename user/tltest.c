#include "lib.h"

void umain() {
	/*
    writef(
        "Smashing some kernel codes...\n"
        "If your implementation is correct, you may see some TOO LOW here:\n"
    );
    *(int *) KERNBASE = 0;
    writef("My mission completed!\n");*/
	
	int envid = syscall_getenvid();
	writef("%d\n", envid);
	//syscall_mem_alloc
	if (syscall_mem_alloc(envid, 0x81000000, PTE_R & PTE_V) < 0) {
		writef("mem_alloc wrong!\n");
	}
	//syscall_mem_map
	
	if (syscall_mem_map(envid, 0x50000000, envid, 0x60000000, PTE_R & PTE_V) < 0) {
		writef("mem_map wrong!\n");
	}
	//syscall_mem_unmap
	
	if (syscall_mem_unmap(envid, 0x50000000) < 0) {
		writef("mem_unmap wrong!\n");
	}
	//syscall_yield
	/*
	int i = 0;
	while(1){
		writef("%d\n", i);
		syscall_yield();
	}
	writef("123\n");
*/
	*(int *) KERNBASE = 0;
}
