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
    
    outb(master_mask, MASTER_DATA);//masking our data ports
    outb(slave_mask, SLAVE_DATA);
    
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_DATA);
    outb(ICW3_MASTER, MASTER_DATA);
    
    outb(ICW4, MASTER_DATA);

    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_DATA);
    outb(ICW3_SLAVE, SLAVE_DATA);
    outb(ICW4, SLAVE_DATA);
    
    enable_irq(IRQ2);

    //no need to restore bc first time
    //outb(MASTER_DATA, a1);
	//outb(SLAVE_DATA, a2);

    //spin_unlock_irqrestore(&i8259_lock, flags); //dont need

    
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint16_t port;
    //uint8_t value;
     //for intel, port and value are flipped when using outb
    if(irq_num < PICCheck){
        port = MASTER_DATA;
        master_mask = inb(port) & ~(1 << irq_num);
        outb(master_mask,port);
              
    }
    else{
        port = SLAVE_DATA;
        irq_num -= PICCheck; 
        slave_mask = inb(port) & ~(1 << irq_num);
        outb(slave_mask,port);
    }
   // value = inb(port) & ~(1 << irq_num);
    //outb(port, value); 
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
    //uint8_t value;
    //for intel, port and value are flipped when using outb
    if(irq_num < PICCheck){
        port = MASTER_DATA;
        master_mask = inb(port) | (1 << irq_num);
        outb(master_mask,port);
    }
    else{
        port = SLAVE_DATA;
        irq_num -= PICCheck;
        slave_mask = inb(port) | (1 << irq_num);
        outb(slave_mask,port);
          
    }
    //value = inb(port) | (1 << irq_num);
    //outb(port, value);   

    
    
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    int holdOr;
    //changed intel outb order
    if (irq_num < PICCheck){//if IRQ came from master, then issue command to master
        holdOr = irq_num | EOI;
        outb(holdOr, MASTER_8259_PORT); //only master
    }
    else{ // if slave then both 
    //eoi for slave 
        holdOr = IRQ2 | EOI; //this is for finding correct master pin
        outb(holdOr, MASTER_8259_PORT); // send to master

        irq_num -= PICCheck; //for slave pin calc
        holdOr = irq_num | EOI; //this is now new irq_num decremented by PICCheck
        outb(holdOr, SLAVE_8259_PORT); //both master and slave
    }
}
