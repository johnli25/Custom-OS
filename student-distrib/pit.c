#include "pit.h"
#include "i8259.h"
#include "lib.h"
#include "scheduling.h"

/*Check this link:
https://wiki.osdev.org/Pit#Operating_Modes */

void initialize_PIT(void){
    outb(CMD_BYTE, IO_PORT_NUM);             /* Set our command byte 0x36 */
    outb(PIT_FREQ & 0xFF, CHANNEL_0);   /* Set low byte of divisor */
    outb(PIT_FREQ >> 8, CHANNEL_0);     /* Set high byte of divisor */
    //outb( , IO_PORT_NUM); //what is this?
    enable_irq(PIT_IRQ_NUM);
}   

void interrupt_PIT(void){
    send_eoi(PIT_IRQ_NUM);

}



