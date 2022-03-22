#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

/* Checkpoint 1 tests */

/* Enable IRQ Master Test
 * 
 * Asserts that enable IRQ can enable port on master
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: enable_irq
 * Files: i8259.c
 */

int enable_irq_test_master(){
	TEST_HEADER;

	int result = PASS;
	enable_irq(5); //a master PIC port to test enable_irq
	return result;

}

/* Enable IRQ Slave Test
 * 
 * Asserts that enable IRQ can enable port on slave
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: enable_irq
 * Files: i8259.c
 */
int enable_irq_test_slave(){
	TEST_HEADER;
	
	int result = PASS;
	enable_irq(10); //a slave PIC port to test enable_irq
	return result;
}
/* Invalid Enable IRQ Test
 * 
 * Asserts that enable IRQ will recognize wrong port
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 * Coverage: enable_irq
 * Files: i8259.c
 */
int invalid_enable_irq_test(){
	TEST_HEADER;
	
	int result = FAIL;
	enable_irq(50); //an out of bounds port that does not exist
	return result;
}

/* Disable IRQ Master Test
 * 
 * Asserts that disable IRQ can enable port on master
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: disable_irq
 * Files: i8259.c
 */
int disable_irq_test_master(){
	TEST_HEADER;

	int result = PASS;
	disable_irq(5); //a master PIC port to test disable_irq
	return result;

}

/* Disable IRQ Slave Test
 * 
 * Asserts that disable IRQ can enable port on slave
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: disable_irq
 * Files: i8259.c
 */
int disable_irq_test_slave(){
	TEST_HEADER;
	
	int result = PASS;
	disable_irq(10); //a slave PIC port to test disable_irq
	return result;
}

/* Invalid Disable IRQ Test
 * 
 * Asserts that disable IRQ can recognize out of bounds port
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 * Coverage: disable_irq
 * Files: i8259.c
 */
int invalid_disable_irq_test(){
	TEST_HEADER;
	
	int result = FAIL;
	disable_irq(50); //an out of bounds port that does not exist
	return result;
}

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* Dereference null pointer Test - Example
 * 
 * Description: dereference null ptr
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: produces page fault exception
 * Coverage: Load IDT, IDT definition, paging check, memory addresses check
 * Files: paging.h/.s, idt.c/idt.h
 */
int null_check_test(){
	//TEST_HEADER;
	//printf("Dereferencing NULL ptr.\n");
	//int result = PASS;
	int * ptr = NULL;
	int val = *ptr;
	//val = PASS;
	return PASS; //raise exception
}

/* Paging Test 2 - Example
 * 
 * Description: accesses a video memory page (in first 4 MB)
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: none
 * Coverage: Load IDT, IDT definition, paging check, memory addresses check
 * Files: paging.h/.s, idt.c/idt.h
 */
int page_test2(){
	//TEST_HEADER;
	int * ptr = (int *)0xb8000; //start of video mem address
	int val = *ptr; //deref * ptr
	//printf("Lower bound of video memory.\n");
	return PASS;
}

/* (Invalid) Paging Test 3 - Example
 * 
 * Description: accesses a video memory page (in first 4 MB)
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: produces page fault exception
 * Coverage: Load IDT, IDT definition, paging check, memory addresses check
 * Files: paging.h/.s, idt.c/idt.h
 */
int invalid_page_test3(){
	TEST_HEADER;
	int result = FAIL;
	int * ptr = (int *)0xb9000; //end of video mem address
	int val = *ptr; //deref * ptr
	printf("Upper bound of video memory.\n");
	return result;
}

/* (Invliad) Paging Test 4 - Example
 * 
 * Description: accesses a video memory page outside of 0 MB-8 MB
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: produces page fault exception
 * Coverage: Load IDT, IDT definition, paging check, memory addresses check
 * Files: paging.h/.s, idt.c/idt.h
 */
int invalid_page_test4(){
	TEST_HEADER;
	int result = FAIL;
	int * ptr = (int *)0x00800391; //random out of bounds memory address (not paged)
	int val = *ptr; //deref * ptr
	printf("Out of bounds paging.\n");
	return result;	
}

/* Regular Paging Test  - Example
 * 
 * Description: accesses a video memory page and kernel page
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: none
 * Coverage: Load IDT, IDT definition, paging check, memory addresses check
 * Files: paging.h/.s, idt.c/idt.h
 */
int paging_test(){
	TEST_HEADER;
	int result = PASS;
	int * vidmem_ptr = (int *)0xb8391; //video mem page
	int * kernel_ptr = (int *)0x00400391; //kernel page
	printf("Were any exceptions or weird stuff triggered? Nope. \n");
	return PASS;
}

/* Divide by 0 test  - Example
 * 
 * Description: takes two ints and divides by 0
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: produces divide by 0 exception
 * Coverage: Load IDT, IDT definition
 * Files: idt.c/idt.h
 */
int divide_by_zero_test(){
	TEST_HEADER;
	volatile int divisor = 0;
	int quotient = 391 / divisor;
	int res = quotient;
	res = PASS;
	return res;
}

/* ALL exceptions tests  - Example
 * 
 * Description: produces any exception when passing in a specific int argument
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: produces an exception based on passed in argument
 * Coverage: Load IDT, IDT definition
 * Files: idt.c/idt.h
 */
int basic_exception_test(int n){
	TEST_HEADER;
	if (n == 1)
		asm volatile("INT $0x01");
	if (n == 2)
		asm volatile("INT $0x02");
	if (n == 3)
		asm volatile("INT $0x03");
	if (n == 4)
		asm volatile("INT $0x04");
	if (n == 5)
		asm volatile("INT $0x05");
	if (n == 6)
		asm volatile("INT $0x06");		
	if (n == 7)
		asm volatile("INT $0x07");
	if (n == 8)
		asm volatile("INT $0x08");	
	if (n == 9)
		asm volatile("INT $0x09");	
	if (n == 0x0A)
		asm volatile("INT $0x0A");	
	if (n == 0x0B)
		asm volatile("INT $0x0B");	
	if (n == 0x0C)
		asm volatile("INT $0x0C");		
	if (n == 0x0D)
		asm volatile("INT $0x0D");	
	if (n == 0x0E)
		asm volatile("INT $0x0E");	
	if (n == 0x0F)
		asm volatile("INT $0x0F");	
	if (n == 0x10)
		asm volatile("INT $0x10");	
	if (n == 0x11)
		asm volatile("INT $0x11");	
	if (n == 0x12)
		asm volatile("INT $0x12");	
	if (n == 0x13)
		asm volatile("INT $0x13");	
	if (n == 0x14)
		asm volatile("INT $0x14");	
	if (n == 0x15)
		asm volatile("INT $0x15");	
	if (n == 0x80)
		asm volatile("INT $0x80");		

	return PASS;
}

// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point 
 * 
 * Description: test suite
 * Inputs: depends
 * Outputs: PASS/FAIL or none
 * Side Effects: produces exceptions, prints things to terminal, etc.
 * Coverage: Load IDT, IDT definition, gdt, paging, rtc, PIC, keyboard
 * Files: pretty much all of them in /student-distrib
 */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	//TEST_OUTPUT("Divide by 0 test", divide_by_zero_test());
	//TEST_OUTPUT("Other exceptions or sys call (basic) test", basic_exception_test(0xE));
	//TEST_OUTPUT("VALID PAGING", paging_test()); 
	TEST_OUTPUT("PIC tests", invalid_enable_irq_test());
}
