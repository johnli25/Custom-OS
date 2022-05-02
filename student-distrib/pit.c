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
    pit_count++;
    // schedTerm++;
    // schedTerm = schedTerm % 3;
    //int current_pid = getProgNum();
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
    case 2:
        if (multi_terms[2].bootup_flag == 0){ //checks if the flag is 0
            //currTerm = 2;          
            switch_terms(2); //MAGIC: switches to the SECOND terminal
            execute((uint8_t*)"shell");

            /*context switch for terminal switch*/
            // int currProgram = getProgNum();

            // pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (currProgram + 1))); //save current process PCB
            // multi_terms[currTerm].curr_proc = mypcb;

            // pcb_t * nextpcb = multi_terms[2].curr_proc;

            // multi_terms[currTerm].curr_proc = mypcb;
            // contextSwitch(mypcb, nextpcb);
            /*end of context switch for terminals*/
            multi_terms[2].bootup_flag = 1; //sets flag as present
        }
        break;
    case 3:
        if (multi_terms[0].bootup_flag == 0){ //checks if flag is present
            //currTerm = 0;
            switch_terms(0); //MAGIC: switches to ZEROth (THIRD) terminal
            execute((uint8_t*)"shell");
            multi_terms[0].bootup_flag = 1; //marks flag as present 
        }
        break;
    default:
        return;
        //break;
    }

    // if (multi_terms[schedTerm].progRunning == 1)
    scheduler();

}



