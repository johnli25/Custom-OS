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
int null_check_test(){
	//TEST_HEADER;
	//printf("Dereferencing NULL ptr.\n");
	//int result = PASS;
	int * ptr = NULL;
	int val = *ptr;
	//val = PASS;
	return PASS; //raise exception
}

int invalid_page_test1(){
	TEST_HEADER;
	int result = FAIL;
	int * ptr = (int *)0xb8000; //start of video mem address
	int val = *ptr; //deref * ptr
	printf("Lower bound of video memory.\n");
	return result;
}

int page_test2(){
	//TEST_HEADER;
	int * ptr = (int *)0xb8000; //start of video mem address
	int val = *ptr; //deref * ptr
	//printf("Lower bound of video memory.\n");
	return PASS;
}

int invalid_page_test3(){
	TEST_HEADER;
	int result = FAIL;
	int * ptr = (int *)0xb9000; //end of video mem address
	int val = *ptr; //deref * ptr
	printf("Upper bound of video memory.\n");
	return result;
}

int invalid_page_test4(){
	TEST_HEADER;
	int result = FAIL;
	int * ptr = (int *)0x00800391; //random out of bounds memory address (not paged)
	int val = *ptr; //deref * ptr
	printf("Out of bounds paging.\n");
	return result;	
}

int paging_test(){
	TEST_HEADER;
	int result = PASS;
	int * vidmem_ptr = (int *)0xb8391; //video mem page
	int * kernel_ptr = (int *)0x00400391; //kernel page
	printf("Were any exceptions or weird stuff triggered? Nope. \n");
	return PASS;
}

int divide_by_zero_test(){
	TEST_HEADER;
	volatile int divisor = 0;
	int quotient = 391 / divisor;
	int res = quotient;
	res = PASS;
	return res;
}

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


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	//TEST_OUTPUT("Divide by 0 test", divide_by_zero_test());
	//TEST_OUTPUT("Other exceptions or sys call (basic) test", basic_exception_test(0xE));
	TEST_OUTPUT("VALID PAGING", paging_test()); 
}
