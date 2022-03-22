/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xff; /* IRQs 0-7  */
uint8_t slave_mask = 0xff;  /* IRQs 8-15 */
//masking with 0xff
/* Initialize the 8259 PIC */
/* 
 * i8259_init
 *   DESCRIPTION: Used to initialize the PIC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initializes the PIC device by mapping ports
 */
void i8259_init(void) {
    //auto_eoi is 0
    
    
    
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_DATA);
    outb(ICW3_MASTER, MASTER_DATA);
    
    outb(ICW4, MASTER_DATA);

    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_DATA);
    outb(ICW3_SLAVE, SLAVE_DATA);
    outb(ICW4, SLAVE_DATA);
    //masking after port setups
    outb(master_mask, MASTER_DATA);//masking our data ports
    outb(slave_mask, SLAVE_DATA);
    
    enable_irq(IRQ2);

    //no need to restore bc first time
    //outb(MASTER_DATA, a1);
	//outb(SLAVE_DATA, a2);

    //spin_unlock_irqrestore(&i8259_lock, flags); //dont need

    
}

/* Enable (unmask) the specified IRQ */
/* 
 * enable_irq(uint32_t irq_num)
 *   DESCRIPTION: Used to enable irq 
 *   INPUTS: uint32_t irq_num
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Enables the specified IRQ which is passed in.
 */
void enable_irq(uint32_t irq_num) {
    uint16_t port;
    //uint8_t value;
     //for intel, port and value are flipped when using outb
     //if 0<= irq <8 then master
    if(irq_num < PICCheck && irq_num >= 0){
        port = MASTER_DATA;
        master_mask = inb(port) & ~(1 << irq_num);
        outb(master_mask,port);
              
    }
    //if 8<= irq <16
    else if (irq_num < SlaveCheck && irq_num >= 8){
        port = SLAVE_DATA;
        irq_num -= PICCheck; 
        slave_mask = inb(port) & ~(1 << irq_num);
        outb(slave_mask,port);
    }
    else{
        printf("Out of Bounds IRQ");
    }

   // value = inb(port) & ~(1 << irq_num);
    //outb(port, value); 
}

/* Disable (mask) the specified IRQ */
/* 
 * disable_irq(uint32_t irq_num)
 *   DESCRIPTION: Used to disable irq 
 *   INPUTS: uint32_t irq_num
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Disables the specified IRQ which is passed in.
 */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
    //uint8_t value;
    //for intel, port and value are flipped when using outb
    if(irq_num < PICCheck && irq_num >= 0){
        port = MASTER_DATA;
        master_mask = inb(port) | (1 << irq_num);
        outb(master_mask,port);
    }
    else if (irq_num < SlaveCheck && irq_num >= 8){
        port = SLAVE_DATA;
        irq_num -= PICCheck;
        slave_mask = inb(port) | (1 << irq_num);
        outb(slave_mask,port);
          
    }
    else{
        printf("Out of Bounds IRQ");
    }
    //value = inb(port) | (1 << irq_num);
    //outb(port, value);   

    
    
}

/* Send end-of-interrupt signal for the specified IRQ */
/* 
 * send_eoi(uint32_t irq_num)
 *   DESCRIPTION: Used to send end of interrupt signal
 *   INPUTS: uint32_t irq_num
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: outputs to master or master and slave depending on passed in IRQ.
 */
void send_eoi(uint32_t irq_num) {
    //removing variable
    //int holdOr;
    //changed intel outb order
    if (irq_num < PICCheck && irq_num >= 0){//if IRQ came from master, then issue command to master
        //holdOr = irq_num | EOI;
        outb(irq_num | EOI, MASTER_8259_PORT); //only master
    }
    else if (irq_num < SlaveCheck && irq_num >= 8){ // if slave then both 
    //eoi for slave 
        //holdOr = IRQ2 | EOI; //this is for finding correct master pin
        outb(IRQ2 | EOI, MASTER_8259_PORT); // send to master

        irq_num -= PICCheck; //for slave pin calc
        //holdOr = irq_num | EOI; //this is now new irq_num decremented by PICCheck
        outb(irq_num | EOI, SLAVE_8259_PORT); //both master and slave
    }
    else{
        printf("Out of Bounds IRQ");
    }
}
