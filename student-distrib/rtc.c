#include "lib.h"
#include "i8259.h"
#include "rtc.h"

void initialize_RTC(void){
    unsigned long flags;
    cli_and_save(flags);

    //from osdev 
    outb(0x8B, RTC_CMD);	    // select register B (0x8B), and disable NMI
    char prev = inb(RTC_DATA);	    // read the current value of register B
    outb(0x8B, RTC_CMD);	        // set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_DATA);    // write the previous value ORed with 0x40. This turns on bit 6 of register B
    //enable_irq(RTC_IRQ);

    outb(0x8A, RTC_CMD);    // select register A (0x8A), and disable NMI
    outb(0x08, RTC_DATA);   // set the RTC freq = 256 Hz, therefore 16 - log_2(freq) = 16 - 8 = 0x08 


    enable_irq(RTC_IRQ);
    restore_flags(flags);

}

void interrupt_RTC(void){
    unsigned long flags;
    cli_and_save(flags);

    printf("Calling test_interrupts() . . . ");
    test_interrupts();

    //from osdev, to enable multiple interrupts 
    outb(0x0C, RTC_CMD);	// select register C
    inb(RTC_DATA);		// just throw away contents
    send_eoi(RTC_IRQ);

    restore_flags(flags);
}


}



