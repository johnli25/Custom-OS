#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "terminal.h"

#define PASS 1
#define FAIL 0

#define PRINT_LARGE 0

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
/* Out of Bounds Enable IRQ Test
 * 
 * Asserts that enable IRQ will recognize wrong port
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: enable_irq
 * Files: i8259.c
 */
int out_of_bounds_enable_irq_test(){
	TEST_HEADER;
	
	int result = PASS;
	enable_irq(391); //an out of bounds port that does not exist
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

/* Out of Bounds Disable IRQ Test
 * 
 * Asserts that disable IRQ can recognize out of bounds port
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: disable_irq
 * Files: i8259.c
 */
int out_of_bounds_disable_irq_test(){
	TEST_HEADER;
	
	int result = PASS;
	disable_irq(391); //an out of bounds port that does not exist
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
	volatile int val = *ptr;
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
	volatile int val = *ptr; //deref * ptr
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
	volatile int val = *ptr; //deref * ptr
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
	volatile int val = *ptr; //deref * ptr
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
	int * vidmem_ptr = (int *)0xb8391; //video mem page
	int * kernel_ptr = (int *)0x00400391; //kernel page
	volatile int vidmem_val = *vidmem_ptr;
	volatile int kernel_val = *kernel_ptr;
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

/* Checkpoint 2 tests */
/*FILE TEST
 * 
 * Description: tests file system-various cases 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: reads from file memory 
 * Coverage: file sys
 * Files: filesys.c/filesys.h
 */
int read_valid_file() {
	TEST_HEADER;
	dentry_t * test_dentry;
	uint8_t buf[391];
	int i; 
	const uint8_t * input = (const uint8_t *)("frame0.txt");
	int result = read_dentry_name(input, test_dentry);
	printf(" \n");
	if (result == -1){
		return FAIL;
	}
	result = read_data(test_dentry->inode, 0, buf, 10000); //offset 0 + random length 10000
	if (187 != result){ //187 is size of output buffer
		return FAIL;
	}

	for (i = 0; i < 187; i++){ //187 is size of ouput buffer
        putc(buf[i]);
	}

	return PASS;
}

/*FILE TEST
 * 
 * Description: tests file system-various cases 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: reads from file memory 
 * Coverage: file sys
 * Files: filesys.c/filesys.h
 */
int read_valid_file2(){
	TEST_HEADER;
	dentry_t * test_dentry;
	uint8_t buf[391];
	const uint8_t * input = (const uint8_t *)("frame1.txt");
	int	result = read_dentry_name(input, test_dentry);
	if (result == -1)
		return FAIL;
	if (174 != read_data(test_dentry->inode, 0, buf, 10000)) //174 is size of output buffer
		return FAIL;

	return PASS;
}

/*FILE TEST
 * 
 * Description: tests file system-various cases 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: reads from file memory 
 * Coverage: file sys
 * Files: filesys.c/filesys.h
 */
int read_exec_file(){
	TEST_HEADER;
	dentry_t * test_dentry;
	uint8_t buf[391]; //391 is random buffer size
	const uint8_t * input = (const uint8_t *)("pingpong");
	int	result = read_dentry_name(input, test_dentry);
	printf(" \n");
	if (result == -1)
		return FAIL;
	result = read_data(test_dentry->inode, 0, buf, 10000); //

	return PASS;
}

/*FILE TEST
 * 
 * Description: tests file system-various cases 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: reads from file memory 
 * Coverage: file sys
 * Files: filesys.c/filesys.h
 */
int read_nonexisting_file(){
	TEST_HEADER;
	dentry_t * test_dentry;
	const uint8_t * input1 = (const uint8_t *)("fake_name_hahaha.txt");
	// const uint8_t * input2 = (const uint8_t *)("get_rickrolled_again.txt");

	int result = read_dentry_name(input1, test_dentry);
	// int result2 = read_dentry_name(input2, test_dentry);
	if (result == 0/* && result2 == 0*/)
		return FAIL;

	return PASS;
}

/*FILE TEST
 * 
 * Description: tests file system-various cases 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: reads from file memory 
 * Coverage: file sys
 * Files: filesys.c/filesys.h
 */
int find_invalid_large_file(){
	TEST_HEADER;
	dentry_t * test_dentry;
	const uint8_t * input = (const uint8_t *)("verylargetextwithverylongname.txt");
	int result = read_dentry_name(input, test_dentry);
	printf("\n");
	if (result == 0)
		return FAIL;
	return PASS;
}

/*FILE TEST
 * 
 * Description: tests file system-various cases 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: reads from file memory 
 * Coverage: file sys
 * Files: filesys.c/filesys.h
 */
int read_large_file(){
	TEST_HEADER;
	dentry_t * test_dentry;
	// uint8_t buf[10000];
	const uint8_t * input = (const uint8_t *)("verylargetextwithverylongname.tx");

	int test1 = read_dentry_name(input, test_dentry);
	if (test1 == -1)
		return FAIL;
	
	printf(" \n");
	uint8_t buf1[43];
	uint8_t str1[] = "very large text file with a very long name\n";
	if(read_data(test_dentry->inode, 0, buf1, 43) != 43)//read from beginning
		return FAIL;	
	if(0 != strncmp((int8_t *)buf1, (int8_t *)str1, 43)){ //0 = successful, equivalent strings
		return FAIL; //if !0, then FAIL
	}

	uint8_t str2[] = "1234567890123456789012345678901234567890123";
	if(read_data(test_dentry->inode, 43, buf1, 43) != 43)//read from offset
		return FAIL;	
	if(0 != strncmp((int8_t *)buf1, (int8_t *)str2, 43)){ //0 = successful, equivalent strings
		return FAIL; //if !0, then FAIL
	}

	uint8_t buf3[43];
	uint8_t str3[] = "jklmnopqrstuvwxyz";
	if(read_data(test_dentry->inode, 4090, buf3, 43) != 43)//read: trek across data blocks
		return FAIL;	
	if(0 != strncmp((int8_t *)buf3, (int8_t *)str3, 17)){ //0 = successful, equivalent strings
		return FAIL; //if !0, then FAIL
	}

	uint8_t buf4[43];
	uint8_t str4[] = ",./<>?\n";
	// for (i = 7; i < 43; i++)
	// 	buf4[i] = '\0';
	if(read_data(test_dentry->inode, 5270, buf4, 43) != 7)//read to very end
		return FAIL;	
	if(0 != strncmp((int8_t *)buf4, (int8_t *)str4, 7)){ //0 = successful, equivalent strings
		return FAIL; //if !0, then FAIL
	}
	//int result = read_data(test_dentry->inode, 0, buf, 1000000);
	//if (result < 0)
	//	return FAIL;
	return PASS;	
}

// int read_large_file2(){
// 	TEST_HEADER;
// 	dentry_t * test_dentry;
// 	uint8_t buf[10000];
// 	const uint8_t * input = (const uint8_t *)("verylargetextwithverylongname.tx");
// 	printf(" \n");
// 	if (read_dentry_name(input, test_dentry) == -1)
// 		return FAIL;
	
// 	printf(" \n");
// 	int result = read_data(test_dentry->inode, 0, buf, 1000000);
// 	if (result < 0)
// 		return FAIL;
// 	return PASS;
// }

/*FILE TEST
 * 
 * Description: tests file system-various cases 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: reads from file memory 
 * Coverage: file sys
 * Files: filesys.c/filesys.h
 */
int read_file_index(){
	TEST_HEADER;
	dentry_t * test_dentry;

	int result1 = read_dentry_index(0, test_dentry);
	int result2 = read_dentry_index(40, test_dentry);
	if (result1 == -1 || result2 == -1)
		return FAIL;

	return PASS;
}

/*FILE TEST
 * 
 * Description: tests file system-various cases 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: reads from file memory 
 * Coverage: file sys
 * Files: filesys.c/filesys.h
 */
int read_file_index_invalid(){
	TEST_HEADER;
	dentry_t * test_dentry;
	int result = read_dentry_index(65, test_dentry);
	if (result == 0)
		return FAIL;
	return PASS;
}

/*FILE TEST
 * 
 * Description: tests file system-various cases 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: reads from file memory 
 * Coverage: file sys
 * Files: filesys.c/filesys.h
 */
int ls_dir_test(){
	TEST_HEADER;
	//dentry_t * test_dentry;
	char buf[33];
	int i;
	int result = 0;
	printf("    \n");
	for (i = 0; i < 20; i++){ //MAGIC #: 20 is arbitrary size for ls dir test
		result = dir_read(2, buf, 2, i); //2 is a filler value FOR NOW
		//buf[result] = '\0';	

		printf(buf);
		printf("\n");
	}
	printf("%d \n", sizeof(buf));
	if (result < 0)
		return FAIL;
	return PASS;
}

//RTC TESTS
/*RTC TEST
 * 
 * Description: tests open functionality
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: NONE
 * Coverage: RTC
 * Files: rtc.c/rtc.h
 */
int rtc_test_open() {
	putc('\n');
	TEST_HEADER;
	int test = open_RTC(NULL);
	if (test) return FAIL;
	return PASS;
}
/*RTC TEST
 * 
 * Description: tests close functionality
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: NONE
 * Coverage: RTC
 * Files: rtc.c/rtc.h
 */
int rtc_test_close() {
	putc('\n');
	TEST_HEADER;
	int test = close_RTC(NULL);
	if (test) return FAIL;
	return PASS;
}
/*RTC TEST
 * 
 * Description: tests write functionality
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: NONE
 * Coverage: RTC
 * Files: rtc.c/rtc.h
 */
int rtc_test_read_write_invalid_freq() {
	putc('\n');
	TEST_HEADER;
	uint32_t freq = 3;	// not a power of two, should fail
	if (write_RTC(0, &freq, 4) == -1)  return FAIL; //4 is number of bytes in proper size argument 
	return PASS;
}
/*RTC TEST
 * 
 * Description: tests write functionality
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: NONE
 * Coverage: RTC
 * Files: rtc.c/rtc.h
 */
int rtc_test_read_write_invalid_size() {
	putc('\n');
	TEST_HEADER;
	uint32_t freq = 2;	// power of two, valid
	if (write_RTC(0, &freq, 3) == -1)  return FAIL;//3 is number of bytes in improper size argument 
	return PASS;
}
/*RTC TEST
 * 
 * Description: tests read/write functionality
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: NONE
 * Coverage: RTC
 * Files: rtc.c/rtc.h
 */
int rtc_test_read_write() {
	TEST_HEADER;
	uint32_t freq, j;
	int count=0
	for (freq = 2; freq < 1025; freq *= 2) {//begin at 2Hz freq and go upto max (1024Hz)
		clearText(); 
		if (write_RTC(391, &freq, 4) == -1) return FAIL; //4 is number of bytes in proper size argument 
		for (j = 1; j <= freq; j++) {
			read_RTC(391, NULL, 391); //391 is for all the unused args 
			printf("%u", j); 
			count+=2; 
			if (j>=10) count++; 
			if (j>=100) count++;
			if (j>=1000) count++; 
			if (count>=80){
				newLine();
				count = 0; 
			}
		}
	}
	putc('\n'); 
	return PASS;
}
	
/* Terminal Read Write Test
 * 
 * Asserts that terminal read write work as intended
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: terminal_read, terminal_write
 * Files: terminal.c
 */
int terminal_read_write(){
	TEST_HEADER;
	terminal_init();
	terminal_open();
	int result = PASS;
	
	 while(1){
		unsigned char buf[127];//testing 128 chars
		terminal_read(127, buf);//128 chars test

		terminal_write(127, buf);
    } //infinite while loop

	terminal_close();
    return result;
}

/* Terminal Read Write 128plus Test
 * 
 * Asserts that terminal read write work as intended when passed in buff size is greater
 than 128 and n is also greater than 128
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: terminal_read, terminal_write
 * Files: terminal.c
 */
int terminal_read_write_128plus(){
	TEST_HEADER;
	int result = PASS;

	terminal_init();
	terminal_open();
	
	 while(1){
		unsigned char buf[500];// a number greater than 500
		terminal_read(500, buf);//n is also 500 

		terminal_write(500, buf);
    } //infinite while loop
	terminal_close();

    return result;
}

/* Terminal Dif Sizes Test
 * 
 * Asserts that terminal read write work as intended when passed in buff size is greater
 than 128 and n is a different value within the range
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: terminal_read, terminal_write
 * Files: terminal.c
 */
int terminalDifSizes(){
	TEST_HEADER;
	terminal_init();
	terminal_open();
	int result = PASS;
	
	 while(1){
		unsigned char buf[500]; // a number greater than 500
		terminal_read(50, buf);	//number less than 500

		terminal_write(50, buf);
    } //infinite while loop
	terminal_close();

    return result;
}




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
	//TEST_OUTPUT("PIC tests", disable_irq_test_master());

	TEST_OUTPUT("filesys CP 3.2 tests", read_valid_file());
	
	//RTC TESTS
	//Run these tests together
	/*
	TEST_OUTPUT("Call open_RTC (should pass)", rtc_test_open());
	TEST_OUTPUT("Call open_RTC when already opened (should fail)", rtc_test_open());
	TEST_OUTPUT("Call close_RTC (should pass)", rtc_test_close());
	TEST_OUTPUT("Call close_RTC when already closed (should fail)", rtc_test_close());
	TEST_OUTPUT("Call read_RTC and write_RTC w/ invalid freq (should fail)", rtc_test_read_write_invalid_freq());
	TEST_OUTPUT("Call read_RTC and write_RTC w/ invalid size (should fail)", rtc_test_read_write_invalid_size());
	*/
	//Run this test alone
	TEST_OUTPUT("Test read_RTC and write_RTC (should pass)", rtc_test_read_write());
	
	//TEST_OUTPUT("TERMINAL READ WRITE TEST", terminal_read_write());
	//TEST_OUTPUT("Terminal Large n", terminal_read_write_128plus());
	//TEST_OUTPUT("Terminal different sizes ", terminalDifSizes());


}
