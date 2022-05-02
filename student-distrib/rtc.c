#define _ISOC99_SOURCE
#include "lib.h"
#include "i8259.h"
#include "rtc.h"
#include "terminal.h"


volatile int state_data [3] = {0, 0, 0}; //{interrupt (bool), freq (int), open (bool)}

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
    outb(0x06, RTC_DATA);   // set the RTC freq = 1024 Hz, therefore 16 - log_2(freq) = 16 - 10 = 0x06 
 
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

    if (multi_terms[currTerm].relative_frequency==1)
        multi_terms[currTerm].relative_frequency = 2048; 

    multi_terms[currTerm].rtc_counter++;
    if (multi_terms[currTerm].rtc_counter == multi_terms[currTerm].relative_frequency/2) {
        state_data[0] = 1; 
        multi_terms[currTerm].rtc_counter = 0; 
    }
     
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
int32_t open_RTC (const uint8_t* filename){
    if (state_data[2]) return -1; 

    cli();
    //from osdev
    char rate = 0x06;			// set the RTC freq = 1024 Hz, therefore 16 - log_2(freq) = 16 - 10 = 0x06 
    outb(0x8A, RTC_CMD);		// set index to register A, disable NMI
    char prev = inb(RTC_DATA);	// get initial value of register A
    outb(RTC_CMD, 0x8A);		// reset index to A
    outb((prev & 0xF0) | rate, RTC_DATA); //write only our rate to A. Note, rate is the bottom 4 bits.
    multi_terms[currTerm].relative_frequency = 1; 
    state_data[1] = virtFreq; //1024 Hz is default freq 
    state_data[2] = 1; 
    //printf("default_freq: 0x0%x rate: %u \n", state_data[1], rate);
    return 0; 
 }

/* read_RTC
 * Description: Waits for interrupt  
 * Inputs: unused 
 * Outputs: None
 * Return Value: 0 upon success 
 * Side Effects: Set interrupt bool within state_data to true (1)
*/ 
int32_t read_RTC (int32_t fd, void* buf, int32_t nbytes){
    state_data[0] = 0;
    while (!state_data[0]){
        
    }
    
        //while (multi_terms[currTerm].rtc_counter < multi_terms[currTerm].relative_frequency) {
        //    multi_terms[currTerm].rtc_counter++;
        //    //printf("counter: 0x0%x rel_freq: %u \n", multi_terms[currTerm].rtc_counter, multi_terms[currTerm].relative_frequency);
        //}
    
    //multi_terms[currTerm].rtc_counter = 0;
    //state_data[0] = 0;
    return 0;
}

/* write_RTC
 * Description: Sets RTC freq   
 * Inputs: pointer with frequency, size of write (4 bytes) 
 * Outputs: None
 * Return Value: 0 upon success 
 * Side Effects: Set RTC freq to 2Hz  
*/ 
int32_t write_RTC (int32_t fd, const void* buf, int32_t nbytes){
    if (!buf || nbytes!=nByteCheck)  return -1;

    uint32_t freq =  *((uint32_t*)buf);

    if (freq > virtFreq || freq < 2 || freq & (freq - 1)) return -1; //checks if frequency is within bounds
    //MAGIC NUMBER: 2 used to check below bounds for our freq

    //https://stackoverflow.com/questions/994593/how-to-do-an-integer-log2-in-c
    uint32_t log_freq;
    asm ( "\tbsr %1, %0\n"
      : "=r"(log_freq)
      : "r" (freq)
    );

    multi_terms[currTerm].rtc_counter = 0; 
    //char rate = 16 - log_freq;			// set the RTC freq to 16 - log_2(freq) 
    //printf("RATE: 0x0%x FREQ: %u \n", rate, freq);
    outb(0x8A, RTC_CMD);		// set index to register A, disable NMI
    char prev = inb(RTC_DATA);	// get initial value of register A
    outb(RTC_CMD, 0x8A);		// reset index to A
    outb((prev & 0xF0) | 0x06, RTC_DATA); //write only our rate to A. Note, rate is the bottom 4 bits.
    //outb((prev & 0xF0) | rate, RTC_DATA); //write only our rate to A. Note, rate is the bottom 4 bits.
    multi_terms[currTerm].relative_frequency = virtFreq/freq; 
    state_data[1] = freq; 

    return 0; 
}

/* close_RTC
 * Description: Close RTC
 * Inputs: unused
 * Outputs: None
 * Return Value: 0 upon success
 * Side Effects: None
*/
int32_t close_RTC (int32_t fd){
    if (!state_data[2]) return -1; //checks if valid
    state_data[2] = 0; 
    return 0; 
}
