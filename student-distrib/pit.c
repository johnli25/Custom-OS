#include "pit.h"

void initialize_PIT(){
    
    //outb( , IO_PORT_NUM);
    enable_irq(PIT_IRQ_NUM);
}   




