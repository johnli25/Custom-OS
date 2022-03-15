/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    //auto_eoi is 0?
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint16_t port;
    if(irq_num < PICCheck){
        port = MASTER_DATA;
        master_mask = inb(port) | (1 << irq_num);
        outb(port, master_mask);       
    }
    else{
        port = SLAVE_DATA;
        irq_num -= PICCheck;
        slave_mask = inb(port) | (1 << irq_num);
        outb(port, slave_mask);  
    }

}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
    if(irq_num < PICCheck){
        port = MASTER_DATA;
        master_mask = inb(port) & ~(1 << irq_num);
        outb(port, master_mask);       
    }
    else{
        port = SLAVE_DATA;
        irq_num -= PICCheck;
        slave_mask = inb(port) & ~(1 << irq_num);
        outb(port, slave_mask);  
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if (irq_num >= PICCheck) //if IRQ came from Slave, then issue command to both PIC chips
        outb(SLAVE_8259_PORT,ICW2_MASTER);
    outb(MASTER_8259_PORT,ICW2_MASTER);
}
