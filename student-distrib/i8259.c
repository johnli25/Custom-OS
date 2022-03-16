/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xff; /* IRQs 0-7  */
uint8_t slave_mask = 0xff;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    //auto_eoi is 0
    unsigned long flags;
    unsigned char a1, a2;
    a1 = inb(MASTER_DATA);              
	a2 = inb(SLAVE_DATA);

    spin_lock_irqsave(&i8259_lock, flags) //spin lock

    outb(master_mask, MASTER_DATA);//masking our data ports
    outb(slave_mask, SLAVE_DATA);
    
    outb_p(ICW1, MASTER_8259_PORT);
    outb_p(MASTER_8259_PORT+0, MASTER_DATA);
    outb_p(ICW3_MASTER, MASTER_DATA);
    
    outb_p(ICW4, MASTER_DATA);

    outb_p(ICW1, SLAVE_8259_PORT);
    outb_p(ICW2_MASTER + PICCheck, SLAVE_DATA);
    outb_p(ICW3_SLAVE, SLAVE_DATA);
    outb_p(ICW4, SLAVE_DATA);
    
    udelay(100);
    //now restore masks with a1, a2
    outb(MASTER_DATA, a1);
	outb(SLAVE_DATA, a2);

    spin_unlock_irqrestore(&i8259_lock, flags);

    
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t value;
    if(irq_num < PICCheck){
        port = MASTER_DATA;
            
    }
    else{
        port = SLAVE_DATA;
        irq_num -= PICCheck;
          
    }
    value = inb(port) | (1 << irq_num);
    outb(port, value);   

}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t value;
    if(irq_num < PICCheck){
        port = MASTER_DATA;
              
    }
    else{
        port = SLAVE_DATA;
        irq_num -= PICCheck; 
    }
    value = inb(port) & ~(1 << irq_num);
    outb(port, value); 
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if (irq_num >= PICCheck) //if IRQ came from Slave, then issue command to both PIC chips
        outb(SLAVE_8259_PORT,ICW2_MASTER);
    outb(MASTER_8259_PORT,ICW2_MASTER);
}
