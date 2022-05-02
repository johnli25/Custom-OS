#include "pit.h"
// #include "i8259.h"
#include "lib.h"
#include "scheduling.h"
#include "systemCalls.h"
#include "terminal.h"

/*Check this link:
https://wiki.osdev.org/Pit#Operating_Modes */
static int pit_count = 0;

/* initialize_PIT
 * Description: Initializes PIT
 * Inputs: None
 * Outputs: None
 * Return Value: None
 * Side Effects: Initializes the PIT by setting the correct BYTES 
*/ 
void initialize_PIT(void){
    outb(CMD_BYTE, IO_PORT_NUM);             /* Set our command byte 0x36 */
    outb(PIT_FREQ & 0xFF, CHANNEL_0);   /* Set low byte of divisor */
    outb((PIT_FREQ /*& 0xFF00*/)>> 8, CHANNEL_0); //MAGIC NUM 8: Needed to shift by 8 to set High Bit of Divisor - OSDEV
    enable_irq(PIT_IRQ_NUM);
}   

/* interrupt_PIT
 * Description: Creates the interrupt handler for the PIT
 * Inputs: None
 * Outputs: None
 * Return Value: None
 * Side Effects: Checks the flags and switches to the correct corresponding Terminal
*/ 
void interrupt_PIT(void){
    send_eoi(PIT_IRQ_NUM);
    //cli();
    pit_count++;
    //uint32_t esp;
    //uint32_t ebp;
    switch (pit_count)
    {
    // case 0:
    //     if (multi_terms[1].bootup_flag == 0){
    //         //currTerm = 1;
    //         switch_terms(1);
    //         execute((uint8_t*)"shell");
    //         multi_terms[1].bootup_flag = 1;
    //     }
    //     break;
    case 1:
        if (multi_terms[1].bootup_flag == 0){ //checks if the flag is 0
            //currTerm = 1;
            switch_terms(1); //MAGIC: switches to the FIRST terminal
            execute((uint8_t*)"shell");
            multi_terms[1].bootup_flag = 1; //marks the flag as present
        }
        break;
        //return;
    case 2:
        if (multi_terms[2].bootup_flag == 0){ //checks if the flag is 0
            //currTerm = 2;          
            switch_terms(2);
            // esp = EIGHTMB - (EIGHTKB * 0) -4;
            // ebp = EIGHTMB - (EIGHTKB * 0) -4;
            // asm volatile( //taking esp ebp of nextpcb and storing into respective esp ebp registers
            //     "movl %0, %%esp;" //esp contains saved_esp
            //     "movl %1, %%ebp;" //ebp contains saved_ebp
            //     :
            //     : "r"(esp), "r"(ebp)
            //     :"esp", "ebp"   //clobbers esp, ebp
            // );
            execute((uint8_t*)"shell");
            multi_terms[2].bootup_flag = 1;
        }
        break;
        //return;
    case 3:
        if (multi_terms[0].bootup_flag == 0){ //checks if flag is present
            //currTerm = 0;
            switch_terms(0);
            // esp = EIGHTMB - (EIGHTKB * 1) -4;
            // ebp = EIGHTMB - (EIGHTKB * 1) -4;
            // asm volatile( //taking esp ebp of nextpcb and storing into respective esp ebp registers
            //     "movl %0, %%esp;" //esp contains saved_esp
            //     "movl %1, %%ebp;" //ebp contains saved_ebp
            //     :
            //     : "r"(esp), "r"(ebp)
            //     :"esp", "ebp"   //clobbers esp, ebp
            // );
            execute((uint8_t*)"shell");
            multi_terms[0].bootup_flag = 1; //marks flag as present 
        }
        break;
        //return;
    case 4: //swap term0 and term2 that were swapped upon initialization
        if(1){
            terminal_t t0 = multi_terms[0];
            terminal_t t1 = multi_terms[1];
            terminal_t t2 = multi_terms[2];

            multi_terms[0] = t1;
            multi_terms[1] = t2;
            multi_terms[2] = t0;

            currTerm = currTerm; //dummy debug
        }

        break;       
    default:
        //return;
        break;
    }

    // if (multi_terms[schedTerm].progRunning == 1)
    scheduler();
    //sti();

}



