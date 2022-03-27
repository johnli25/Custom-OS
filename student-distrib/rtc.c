#define _ISOC99_SOURCE
#include "lib.h"
#include "i8259.h"
#include "rtc.h"

volatile int state_data [2] = {0, 0}; //{interrupt (bool), freq (int)}

/* initialize_RTC
 * Description: Initializes RTC
 * Inputs: None
 * Outputs: None
 * Return Value: None
 * Side Effects: Enables IRQ and sets control registers 
*/ 
void initialize_RTC(void){

    cli();

    //from osdev 
    outb(0x8B, RTC_CMD);	    // select register B (0x8B), and disable NMI
    char prev = inb(RTC_DATA);	    // read the current value of register B
    outb(0x8B, RTC_CMD);	        // set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_DATA);    // write the previous value ORed with 0x40. This turns on bit 6 of register B

    outb(0x8A, RTC_CMD);    // select register A (0x8A), and disable NMI
    outb(0x0F, RTC_DATA);   // set the RTC freq = 2 Hz, therefore 16 - log_2(freq) = 16 - 1 = 0x0F 


    enable_irq(RTC_IRQ);
}

/* interrupt_RTC
 * Description: Called when interrupt occurs 
 * Inputs: None
 * Outputs: None
 * Return Value: None
 * Side Effects: Flashing random characters covers screen  
*/ 
void interrupt_RTC(void){

    state_data[0] = 1; 

    //printf("Calling test_interrupts() . . . ");
    //test_interrupts();

    //from osdev, to enable multiple interrupts 
    outb(0x0C, RTC_CMD);	// select register C
    inb(RTC_DATA);		// just throw away contents
    send_eoi(RTC_IRQ);
}

/* open_RTC
 * Description: Set RTC freq to 2Hz  
 * Inputs: unused 
 * Outputs: None
 * Return Value: 0 upon success 
 * Side Effects: Set RTC freq to 2Hz  
*/ 
int32_t open_RTC (const uint8 t* filename){
    cli();
    //from osdev
    char rate = 0x0F;			// set the RTC freq = 2 Hz, therefore 16 - log_2(freq) = 16 - 1 = 0x0F 
    outb(0x8A, RTC_CMD);		// set index to register A, disable NMI
    char prev = inb(RTC_DATA);	// get initial value of register A
    outb(RTC_CMD, 0x8A);		// reset index to A
    outb((prev & 0xF0) | rate, RTC_DATA); //write only our rate to A. Note, rate is the bottom 4 bits.
    state_data[1] = 2; 
    return 0; 
 }

/* read_RTC
 * Description: Waits for interrupt  
 * Inputs: unused 
 * Outputs: None
 * Return Value: 0 upon success 
 * Side Effects: Set interrupt bool within state_data to true (1)
*/ 
int32_t read_RTC (int32 t fd, void* buf, int32 t nbytes){
    while (!state_data[0]);
    state_data[0] = 1; 
    return 0;
}

/* write_RTC
 * Description: Sets RTC freq   
 * Inputs: pointer with frequency, size of write (4 bytes) 
 * Outputs: None
 * Return Value: 0 upon success 
 * Side Effects: Set RTC freq to 2Hz  
*/ 
int32_t write_RTC (int32 t fd, const void* buf, int32 t nbytes){
    if (!buf || nbytes!=4)  return -1;

    uint32_t freq =  *((uint32_t*)buf);

    if (freq > 1024 || freq < 2 || freq & (*freq - 1)) return -1; 

    //https://stackoverflow.com/questions/994593/how-to-do-an-integer-log2-in-c
    uint32_t log_freq;
    asm ( "\tbsr %1, %0\n"
      : "=r"(log_freq)
      : "r" (freq)
    );

    char rate = 16 - log_freq;			// set the RTC freq to 16 - log_2(freq) 
    outb(0x8A, RTC_CMD);		// set index to register A, disable NMI
    char prev = inb(RTC_DATA);	// get initial value of register A
    outb(RTC_CMD, 0x8A);		// reset index to A
    outb((prev & 0xF0) | rate, RTC_DATA); //write only our rate to A. Note, rate is the bottom 4 bits.
    state_data[1] = freq; 

    return 0; 
}

int32_t close_RTC (int32 t fd){
    return 0; 
}
